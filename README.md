# Лабораторная работа № 06

## Тема: Паттерны проектирования

## Описание

Редактор подземелья для RPG игры Balagur Fate 3. Программа позволяет моделировать базовые локации, расставляя на них NPC (non-player characters) и проводить боевые взаимодействия между ними.

## Требования

1. **Паттерны проектирования:**
   - **Factory Pattern**: Для создания NPC и загрузки NPC из файла
   - **Visitor Pattern**: Для проведения цикла сражения
   - **Observer Pattern**: Для печати событий о совершенных убийствах
     - `ConsoleObserver`: Вывод на экран
     - `FileObserver`: Запись в файл "log.txt"

2. **3 типа NPC:**
   - **Bear** (Медведь)
   - **Elf** (Эльф)
   - **Robber** (Разбойник)

3. **Правила боя:**
   - Медведь убивает Эльфов
   - Эльф убивает Разбойников
   - Разбойник убивает Разбойников

4. **Функциональность:**
   - Добавление NPC по координатам (0-500) с уникальными именами
   - Сохранение объектов в файл и загрузка из файла
   - Печать перечня объектов на экран (тип, координаты, имя)
   - Запуск боевого режима с указанием дальности боя

## Структура проекта

```
lab_6/
├── include/          # Заголовочные файлы
│   ├── npc.hpp       # Базовый класс NPC
│   ├── bear.hpp      # Класс Bear
│   ├── elf.hpp       # Класс Elf
│   ├── robber.hpp    # Класс Robber
│   ├── npc_factory.hpp
│   ├── fight_visitor.hpp
│   └── observer.hpp
├── src/              # Исходные файлы
│   ├── main.cpp      # Точка входа
│   ├── npc.cpp
│   ├── bear.cpp
│   ├── elf.cpp
│   ├── robber.cpp
│   ├── npc_factory.cpp
│   └── observer.cpp
├── tests/            # Тесты
│   ├── test_npc.cpp
│   ├── test_factory.cpp
│   └── test_visitor.cpp
├── CMakeLists.txt    # Файл сборки
└── README.md         # Этот файл
```

## Сборка

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows (PowerShell)

```powershell
mkdir build
cd build
cmake ..
cmake --build .
```

### Windows (Visual Studio)

```powershell
mkdir build
cd build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release
```

## Запуск

### Linux/macOS

```bash
cd build
./lab6_main
```

### Windows

```powershell
cd build
.\lab6_main.exe
# или
.\Release\lab6_main.exe
# или
.\Debug\lab6_main.exe
```

## Тестирование

### Запуск тестов

```bash
cd build
ctest
# или
ctest --verbose
```

### Запуск конкретного теста

```bash
cd build
./lab6_tests
```

## Особенности реализации

### Factory Pattern

`NpcFactory` предоставляет методы:
- `CreateNPC(NpcType, name, x, y)` - создание NPC по типу
- `CreateNPC(std::istream&)` - создание NPC из потока
- `SaveToFile()` - сохранение NPC в файл
- `LoadFromFile()` - загрузка NPC из файла

### Visitor Pattern

`FightVisitor` используется для реализации боевой логики:
- Каждый NPC реализует `Accept()` для принятия посетителя
- Каждый NPC реализует `Visit()` для обработки боя с конкретным типом

### Observer Pattern

Два типа наблюдателей:
- `ConsoleObserver` - выводит события убийств на экран
- `FileObserver` - записывает события в файл "log.txt"

NPC могут подписаться на несколько наблюдателей через `Subscribe()`.

## Пример использования

Программа демонстрирует:
1. Создание NPC различных типов
2. Сохранение NPC в файл
3. Загрузку NPC из файла
4. Проведение боев с различными дальностями
5. Генерацию случайных NPC и проведение нескольких раундов боев

## Выходные файлы

- `npcs.txt` - файл с сохраненными NPC
- `log.txt` - лог файл с событиями убийств

## Примечания

- NPC размещаются в квадрате 0 <= x <= 500, 0 <= y <= 500
- У каждого NPC должно быть уникальное имя
- В боевом режиме NPC сражаются каждый с каждым, если дальность позволяет
- Проигравший NPC удаляется (может погибнуть и оба NPC)
