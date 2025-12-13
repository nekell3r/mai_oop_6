# Архитектура и объяснение кода Lab 6

## Обзор проекта

Проект реализует систему боев между NPC (Non-Player Characters) в подземелье. Используются три паттерна проектирования:
- **Factory** - для создания NPC
- **Visitor** - для определения результатов боев
- **Observer** - для уведомления о боях

## Структура проекта

```
lab_6/
├── include/          # Заголовочные файлы
│   ├── npc.hpp       # Базовый класс NPC
│   ├── bear.hpp      # Класс Bear
│   ├── elf.hpp       # Класс Elf
│   ├── robber.hpp    # Класс Robber
│   ├── fight_visitor.hpp  # Интерфейс Visitor для боев
│   ├── observer.hpp  # Интерфейс Observer и реализации
│   └── npc_factory.hpp    # Фабрика для создания NPC
├── src/              # Реализации
│   ├── npc.cpp
│   ├── bear.cpp
│   ├── elf.cpp
│   ├── robber.cpp
│   ├── observer.cpp
│   ├── npc_factory.cpp
│   └── main.cpp      # Точка входа
└── tests/            # Unit-тесты
```

---

## Детальное объяснение компонентов

### 1. Базовый класс NPC (`npc.hpp`, `npc.cpp`)

**Назначение:** Абстрактный базовый класс для всех типов NPC.

**Ключевые особенности:**

#### Наследование от `std::enable_shared_from_this<NPC>`
```cpp
class NPC : public std::enable_shared_from_this<NPC>
```
Это позволяет объекту NPC безопасно получить `shared_ptr` на самого себя. Это нужно для паттерна Visitor, где NPC должен передать себя в метод `Visit()`.

**Зачем это нужно:**
- В методе `Accept()` NPC должен передать себя как `shared_ptr`
- Обычный `this` - это сырой указатель, а нам нужен `shared_ptr`
- `shared_from_this()` создает `shared_ptr` из текущего объекта

#### Защищенные поля:
```cpp
protected:
  std::string name_;      // Имя NPC
  int x_, y_;            // Координаты на карте
  NpcType type_;         // Тип NPC (Bear, Elf, Robber)
  std::vector<std::shared_ptr<IFightObserver>> observers_;  // Список наблюдателей
```

**Почему `observers_` в `protected`:**
- Наследники могут напрямую работать с наблюдателями, если нужно
- Но обычно используется через публичные методы `Subscribe()` и `FightNotify()`

#### Важно: методы Fight убраны из абстрактного класса
**Почему это правильно:**
- Абстрактный класс `NPC` не должен знать о конкретных типах (Bear, Elf, Robber)
- Это нарушает принцип абстракции и делает код менее гибким
- Вся логика боя теперь находится в паттерне Visitor (методы `Visit()`)
- `NPC` остается чистым абстрактным интерфейсом без зависимостей от конкретных типов

#### Метод `Accept()` - часть паттерна Visitor:
```cpp
virtual bool Accept(std::shared_ptr<FightVisitor> visitor) = 0;
```

**Как работает:**
1. NPC получает Visitor (который является другим NPC)
2. NPC вызывает `visitor->Visit(this)` с правильным типом
3. Visitor (другой NPC) определяет, может ли он убить этого NPC

**Почему это нужно:**
- Позволяет избежать огромного количества `if-else` или `switch-case`
- Каждый тип NPC знает, как правильно передать себя Visitor'у
- Visitor (атакующий NPC) знает правила боя для каждого типа

#### Метод `IsClose()`:
```cpp
bool IsClose(const std::shared_ptr<NPC>& other, size_t distance) const;
```

**Реализация:**
```cpp
bool NPC::IsClose(const std::shared_ptr<NPC>& other, size_t distance) const {
  auto dx = x_ - other->x_;
  auto dy = y_ - other->y_;
  return (dx * dx + dy * dy) <= static_cast<int>(distance * distance);
}
```

**Оптимизация:**
- Используется квадрат расстояния вместо `sqrt()` для экономии вычислений
- `distance * distance` сравнивается с `dx² + dy²`
- Если `dx² + dy² ≤ distance²`, то NPC находятся близко

#### Observer паттерн:
```cpp
void Subscribe(std::shared_ptr<IFightObserver> observer);
void FightNotify(const std::shared_ptr<NPC>& attacker, 
                 const std::shared_ptr<NPC>& defender, 
                 bool win);
```

**Как работает:**
1. Создаются наблюдатели (например, `ConsoleObserver`, `FileObserver`)
2. NPC подписывается на них через `Subscribe()`
3. При бое вызывается `FightNotify()`, который уведомляет всех наблюдателей
4. Наблюдатели выводят информацию о бое в консоль/файл

**Преимущества:**
- NPC не знает, КАК обрабатывается информация о бое
- Можно добавить новые типы наблюдателей без изменения NPC
- Разделение ответственности: NPC - логика боя, Observer - логирование

---

### 2. Конкретные типы NPC (Bear, Elf, Robber)

Все три класса имеют одинаковую структуру, но разные правила боя.

#### Структура класса (на примере Bear):
```cpp
class Bear : public NPC, public FightVisitor {
 public:
  Bear(const std::string& name, int x, int y);
  
  bool Accept(std::shared_ptr<FightVisitor> visitor) override;
  
  // Методы Fight определяют, может ли Bear убить противника
  bool Fight(std::shared_ptr<Bear> other) override;
  bool Fight(std::shared_ptr<Elf> other) override;
  bool Fight(std::shared_ptr<Robber> other) override;
  
  // Методы Visit определяют, может ли Bear убить защитника
  bool Visit(std::shared_ptr<Bear> defender) override;
  bool Visit(std::shared_ptr<Elf> defender) override;
  bool Visit(std::shared_ptr<Robber> defender) override;
};
```

**Почему множественное наследование?**
- `public NPC` - Bear является NPC
- `public FightVisitor` - Bear может быть Visitor'ом (атакующим)

#### Реализация `Accept()`:
```cpp
bool Bear::Accept(std::shared_ptr<FightVisitor> visitor) {
  return visitor->Visit(std::dynamic_pointer_cast<Bear>(shared_from_this()));
}
```

**Разбор по шагам:**
1. `shared_from_this()` - получаем `shared_ptr<NPC>` на текущий объект
2. `std::dynamic_pointer_cast<Bear>(...)` - приводим к `shared_ptr<Bear>`
3. `visitor->Visit(...)` - вызываем метод Visitor'а с правильным типом
4. Visitor (другой NPC) определяет результат боя

**Почему `dynamic_pointer_cast`?**
- Проверяет, что объект действительно является Bear
- Если нет - вернет `nullptr` (но в нашем случае это всегда Bear)

#### Правила боя (из `bear.cpp`):
```cpp
// Bear kills Elves - логика в методах Visit
bool Bear::Visit(std::shared_ptr<Bear> defender) {
  return false;  // Bear не убивает Bear
}

bool Bear::Visit(std::shared_ptr<Elf> defender) {
  return true;  // Bear убивает Elf
}

bool Bear::Visit(std::shared_ptr<Robber> defender) {
  return false;  // Bear не убивает Robber
}

**Полная таблица правил:**
- **Bear** убивает: Elf
- **Elf** убивает: Robber
- **Robber** убивает: Robber (включая себя)

#### Реализация `Visit()`:
```cpp
bool Bear::Visit(std::shared_ptr<Elf> defender) {
  return true;  // Bear убивает Elf - логика боя здесь
}
```

**Почему логика в `Visit()`?**
- `Visit()` получает уже правильно типизированный `shared_ptr`
- Вся логика боя инкапсулирована в Visitor паттерне
- Абстрактный класс `NPC` не знает о конкретных типах

---

### 3. Паттерн Visitor (`fight_visitor.hpp`)

**Назначение:** Определяет интерфейс для "посещения" разных типов NPC.

```cpp
class FightVisitor {
 public:
  virtual ~FightVisitor() = default;
  
  virtual bool Visit(std::shared_ptr<Bear> defender) = 0;
  virtual bool Visit(std::shared_ptr<Elf> defender) = 0;
  virtual bool Visit(std::shared_ptr<Robber> defender) = 0;
};
```

**Как работает паттерн Visitor:**

1. **Атакующий NPC** (например, Bear) является Visitor'ом
2. **Защищающийся NPC** (например, Elf) вызывает `Accept(visitor)`
3. Защищающийся NPC передает себя Visitor'у с правильным типом
4. Visitor определяет результат боя на основе типа защитника

**Пример потока вызовов:**
```
Bear атакует Elf:
1. bear->Accept(elf_visitor)  // Elf вызывает Accept
2. elf->Accept(bear)          // Elf передает себя Bear'у
3. bear->Visit(elf)           // Bear определяет результат
4. bear->Fight(elf)           // Bear проверяет правила
5. return true                // Bear убивает Elf
```

**Преимущества Visitor:**
- Избегаем `if-else` цепочек типа `if (attacker is Bear && defender is Elf)`
- Каждый тип NPC инкапсулирует свои правила боя
- Легко добавить новый тип NPC (нужно добавить методы в Visitor и NPC)

---

### 4. Паттерн Observer (`observer.hpp`, `observer.cpp`)

**Назначение:** Уведомление о событиях боя без жесткой связи.

#### Интерфейс:
```cpp
class IFightObserver {
 public:
  virtual ~IFightObserver() = default;
  virtual void OnFight(const std::shared_ptr<NPC>& attacker,
                       const std::shared_ptr<NPC>& defender,
                       bool win) = 0;
};
```

#### Реализации:

**ConsoleObserver:**
```cpp
void ConsoleObserver::OnFight(..., bool win) {
  if (win) {
    std::cout << "MURDER: " << attacker_type << " killed " << defender_type;
  }
}
```

**FileObserver:**
```cpp
void FileObserver::OnFight(..., bool win) {
  if (win && log_file_.is_open()) {
    log_file_ << "MURDER: ...";
  }
}
```

**Как работает:**
1. Создаются наблюдатели: `auto console_obs = std::make_shared<ConsoleObserver>();`
2. NPC подписывается: `bear->Subscribe(console_obs);`
3. При бое: `bear->FightNotify(attacker, defender, true);`
4. Все наблюдатели получают уведомление и обрабатывают его

**Преимущества:**
- NPC не знает, КАК обрабатывается информация
- Можно добавить новые наблюдатели (например, NetworkObserver) без изменения NPC
- Несколько наблюдателей могут обрабатывать одно событие

---

### 5. Паттерн Factory (`npc_factory.hpp`, `npc_factory.cpp`)

**Назначение:** Централизованное создание объектов NPC.

#### Основные методы:

**1. Создание по типу:**
```cpp
static std::shared_ptr<NPC> CreateNPC(NpcType type, 
                                      const std::string& name, 
                                      int x, int y);
```

**Реализация:**
```cpp
switch (type) {
  case NpcType::Bear:
    return std::make_shared<Bear>(name, x, y);
  case NpcType::Elf:
    return std::make_shared<Elf>(name, x, y);
  case NpcType::Robber:
    return std::make_shared<Robber>(name, x, y);
  default:
    throw std::invalid_argument("Unknown NPC type");
}
```

**2. Создание из потока:**
```cpp
static std::shared_ptr<NPC> CreateNPC(std::istream& is);
```

**Как работает:**
- Читает тип, имя, координаты из потока
- Вызывает `CreateNPC(type, name, x, y)`

**3. Сохранение в файл:**
```cpp
static void SaveToFile(const std::vector<std::shared_ptr<NPC>>& npcs,
                       const std::string& filename);
```

**Формат файла:**
```
3                    // Количество NPC
1 Bear1 100 100     // Тип Имя X Y
2 Elf1 110 110
3 Robber1 120 120
```

**4. Загрузка из файла:**
```cpp
static std::vector<std::shared_ptr<NPC>> LoadFromFile(const std::string& filename);
```

**Преимущества Factory:**
- Централизованное создание объектов
- Легко добавить валидацию (например, проверка координат)
- Инкапсулирует знание о том, как создавать разные типы NPC

---

### 6. Логика боев (`main.cpp` - функция `Fight()`)

**Назначение:** Определяет, какие NPC сражаются и кто выживает.

```cpp
std::vector<std::shared_ptr<NPC>> Fight(
    const std::vector<std::shared_ptr<NPC>>& npcs,
    size_t distance) {
  std::set<std::shared_ptr<NPC>> dead_list;
  std::set<std::pair<std::shared_ptr<NPC>, std::shared_ptr<NPC>>> processed_pairs;
  
  // Перебираем все пары NPC
  for (const auto& attacker : npcs) {
    for (const auto& defender : npcs) {
      if (attacker == defender) continue;  // Пропускаем самого себя
      
      // Избегаем обработки одной пары дважды
      auto pair1 = std::make_pair(attacker, defender);
      auto pair2 = std::make_pair(defender, attacker);
      if (processed_pairs.find(pair1) != processed_pairs.end() ||
          processed_pairs.find(pair2) != processed_pairs.end()) {
        continue;
      }
      
      // Проверяем расстояние
      if (attacker->IsClose(defender, distance)) {
        processed_pairs.insert(pair1);
        
        // Проверяем, может ли attacker убить defender
        auto attacker_visitor = std::dynamic_pointer_cast<FightVisitor>(attacker);
        if (attacker_visitor && defender->Accept(attacker_visitor)) {
          attacker->FightNotify(attacker, defender, true);
          dead_list.insert(defender);
        }
        
        // Проверяем, может ли defender убить attacker (взаимный бой)
        auto defender_visitor = std::dynamic_pointer_cast<FightVisitor>(defender);
        if (defender_visitor && attacker->Accept(defender_visitor)) {
          defender->FightNotify(defender, attacker, true);
          dead_list.insert(attacker);
        }
      }
    }
  }
  
  // Возвращаем выживших
  std::vector<std::shared_ptr<NPC>> result;
  for (const auto& npc : npcs) {
    if (dead_list.find(npc) == dead_list.end()) {
      result.push_back(npc);
    }
  }
  
  return result;
}
```

**Разбор по шагам:**

1. **Инициализация:**
   - `dead_list` - множество убитых NPC
   - `processed_pairs` - множество обработанных пар (чтобы не обрабатывать дважды)

2. **Двойной цикл:**
   - Перебираем все пары NPC
   - Пропускаем, если `attacker == defender`

3. **Проверка на дубликаты:**
   - Используем `processed_pairs` чтобы не обрабатывать пару (A, B) и (B, A) дважды
   - Это оптимизация, но также важно для корректности логики

4. **Проверка расстояния:**
   - `attacker->IsClose(defender, distance)` - находятся ли NPC близко

5. **Двусторонний бой:**
   - **Атака attacker → defender:**
     - Приводим attacker к `FightVisitor`
     - Вызываем `defender->Accept(attacker_visitor)`
     - Если вернулось `true`, добавляем defender в `dead_list`
   
   - **Атака defender → attacker:**
     - Аналогично, но наоборот
     - Это позволяет реализовать правило "оба NPC могут погибнуть"

6. **Формирование результата:**
   - Создаем вектор выживших NPC
   - Возвращаем его

**Важные моменты:**

- **Взаимный бой:**** Оба NPC могут атаковать друг друга, поэтому оба могут погибнуть
- **Visitor паттерн:** Используется для определения результата боя
- **Observer паттерн:** Уведомления отправляются только при успешном убийстве

---

### 7. Точка входа (`main.cpp`)

**Структура `main()`:**

1. **Создание наблюдателей:**
```cpp
auto console_obs = std::make_shared<ConsoleObserver>();
auto file_obs = std::make_shared<FileObserver>("log.txt");
```

2. **Создание NPC через Factory:**
```cpp
auto bear = NpcFactory::CreateNPC(NpcType::Bear, "Bear1", 100, 100);
auto elf = NpcFactory::CreateNPC(NpcType::Elf, "Elf1", 110, 110);
auto robber = NpcFactory::CreateNPC(NpcType::Robber, "Robber1", 120, 120);
```

3. **Подписка на наблюдателей:**
```cpp
bear->Subscribe(console_obs);
bear->Subscribe(file_obs);
// ...
```

4. **Сохранение в файл:**
```cpp
NpcFactory::SaveToFile(npcs, "npcs.txt");
```

5. **Загрузка из файла:**
```cpp
auto loaded = NpcFactory::LoadFromFile("npcs.txt");
```

6. **Бой:**
```cpp
auto survivors = Fight(loaded, 50);
```

7. **Генерация случайных NPC и несколько раундов боев:**
```cpp
for (size_t dist = 20; dist <= 100 && !random_npcs.empty(); dist += 20) {
  random_npcs = Fight(random_npcs, dist);
}
```

---

## Связи между компонентами

### Диаграмма наследования:
```
NPC (абстрактный)
  ├── Bear : public NPC, public FightVisitor
  ├── Elf : public NPC, public FightVisitor
  └── Robber : public NPC, public FightVisitor

IFightObserver (интерфейс)
  ├── ConsoleObserver
  └── FileObserver

FightVisitor (интерфейс)
  └── (реализуется Bear, Elf, Robber)
```

### Поток данных при бое:

```
1. Fight() вызывает attacker->IsClose(defender, distance)
   └──> NPC::IsClose() проверяет расстояние

2. attacker приводится к FightVisitor
   └──> dynamic_pointer_cast<FightVisitor>(attacker)

3. defender->Accept(attacker_visitor)
   └──> defender вызывает visitor->Visit(defender_typed)
        └──> attacker->Visit(defender_typed)
             └──> attacker->Fight(defender_typed)
                  └──> возвращает true/false

4. Если true, attacker->FightNotify(attacker, defender, true)
   └──> NPC::FightNotify() итерирует observers_
        └──> observer->OnFight(attacker, defender, true)
             └──> ConsoleObserver::OnFight() выводит в консоль
             └──> FileObserver::OnFight() пишет в файл
```

---

## Паттерны проектирования в деталях

### 1. Factory Pattern

**Проблема:** Нужно создавать объекты разных типов, но не хочется знать детали создания.

**Решение:** Централизованная фабрика `NpcFactory`.

**Преимущества:**
- Инкапсуляция логики создания
- Легко добавить валидацию
- Единая точка создания объектов

**Использование:**
```cpp
auto npc = NpcFactory::CreateNPC(NpcType::Bear, "Bear1", 100, 100);
```

### 2. Visitor Pattern

**Проблема:** Нужно определить результат операции на основе типов ОБОИХ объектов (double dispatch).

**Решение:** Visitor паттерн с двойной диспетчеризацией.

**Как работает:**
1. Атакующий NPC является Visitor'ом
2. Защищающийся NPC вызывает `Accept(visitor)`
3. `Accept()` передает себя Visitor'у с правильным типом
4. Visitor определяет результат на основе типа защитника

**Преимущества:**
- Избегаем огромных `if-else` цепочек
- Каждый тип инкапсулирует свои правила
- Легко расширять

**Использование:**
```cpp
auto visitor = std::dynamic_pointer_cast<FightVisitor>(attacker);
bool can_kill = defender->Accept(visitor);
```

### 3. Observer Pattern

**Проблема:** Нужно уведомлять о событиях, но не хочется жесткой связи.

**Решение:** Observer паттерн с подпиской/уведомлением.

**Как работает:**
1. Создаются наблюдатели
2. NPC подписывается на них
3. При событии NPC уведомляет всех наблюдателей
4. Наблюдатели обрабатывают событие независимо

**Преимущества:**
- Слабая связь между компонентами
- Можно добавить новые наблюдатели без изменения NPC
- Несколько наблюдателей могут обрабатывать одно событие

**Использование:**
```cpp
npc->Subscribe(observer);
npc->FightNotify(attacker, defender, true);
```

---

## Важные детали реализации

### 1. `std::enable_shared_from_this`

**Зачем нужно:**
- Позволяет объекту безопасно получить `shared_ptr` на самого себя
- Нужно для паттерна Visitor, где NPC передает себя в `Visit()`

**Как использовать:**
```cpp
class NPC : public std::enable_shared_from_this<NPC> {
  // ...
  bool Accept(std::shared_ptr<FightVisitor> visitor) {
    return visitor->Visit(
      std::dynamic_pointer_cast<Bear>(shared_from_this())
    );
  }
};
```

**Важно:** Объект должен быть создан через `std::make_shared`, иначе `shared_from_this()` выбросит исключение.

### 2. Double Dispatch (двойная диспетчеризация)

**Проблема:** В C++ нет множественной диспетчеризации (multiple dispatch).

**Решение:** Visitor паттерн эмулирует double dispatch.

**Как работает:**
1. Первая диспетчеризация: компилятор выбирает метод на основе типа `visitor`
2. Вторая диспетчеризация: `Accept()` передает себя с правильным типом, компилятор выбирает правильную перегрузку `Visit()`

### 3. Множественное наследование

**Почему Bear наследуется и от NPC, и от FightVisitor:**
- `NPC` - Bear является NPC (имеет координаты, имя, наблюдателей)
- `FightVisitor` - Bear может быть Visitor'ом (атакующим)

**Альтернатива:** Можно было бы использовать композицию, но наследование проще и эффективнее.

### 4. Оптимизация `IsClose()`

**Проблема:** Вычисление `sqrt(dx² + dy²)` дорогое.

**Решение:** Сравниваем квадраты расстояний:
```cpp
return (dx * dx + dy * dy) <= static_cast<int>(distance * distance);
```

**Почему это работает:**
- Если `a ≤ b`, то `a² ≤ b²` (для неотрицательных чисел)
- Избегаем вычисления квадратного корня

### 5. Предотвращение дублирования пар в `Fight()`

**Проблема:** Пара (A, B) и (B, A) - это одна и та же пара для боя.

**Решение:** Используем `processed_pairs` для отслеживания обработанных пар.

**Как работает:**
```cpp
auto pair1 = std::make_pair(attacker, defender);
auto pair2 = std::make_pair(defender, attacker);
if (processed_pairs.find(pair1) != processed_pairs.end() ||
    processed_pairs.find(pair2) != processed_pairs.end()) {
  continue;  // Пропускаем, если уже обработали
}
```

---

## Примеры использования

### Пример 1: Создание и бой двух NPC

```cpp
// Создание
auto bear = NpcFactory::CreateNPC(NpcType::Bear, "Bear1", 100, 100);
auto elf = NpcFactory::CreateNPC(NpcType::Elf, "Elf1", 110, 110);

// Подписка на наблюдателей
auto console_obs = std::make_shared<ConsoleObserver>();
bear->Subscribe(console_obs);
elf->Subscribe(console_obs);

// Бой
std::vector<std::shared_ptr<NPC>> npcs = {bear, elf};
auto survivors = Fight(npcs, 50);

// Результат: bear выжил, elf убит
// В консоль выведется: "MURDER: Bear \"Bear1\" killed Elf \"Elf1\""
```

### Пример 2: Взаимный бой (оба могут погибнуть)

```cpp
auto robber1 = NpcFactory::CreateNPC(NpcType::Robber, "Robber1", 100, 100);
auto robber2 = NpcFactory::CreateNPC(NpcType::Robber, "Robber2", 105, 105);

// Оба подписаны на наблюдателей
robber1->Subscribe(console_obs);
robber2->Subscribe(console_obs);

// Бой
std::vector<std::shared_ptr<NPC>> npcs = {robber1, robber2};
auto survivors = Fight(npcs, 50);

// Результат: оба могут погибнуть (если оба атакуют успешно)
```

### Пример 3: Сохранение и загрузка

```cpp
// Создание и сохранение
std::vector<std::shared_ptr<NPC>> npcs = {bear, elf, robber};
NpcFactory::SaveToFile(npcs, "npcs.txt");

// Загрузка
auto loaded = NpcFactory::LoadFromFile("npcs.txt");
// loaded содержит те же NPC, что и npcs
```

---

## Заключение

Проект демонстрирует использование трех паттернов проектирования:
- **Factory** - для создания объектов
- **Visitor** - для определения результатов операций на основе типов обоих объектов
- **Observer** - для уведомления о событиях

Все компоненты связаны через интерфейсы и используют полиморфизм для гибкости и расширяемости кода.

