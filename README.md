# RegExp Calculator

[English](#english) | [Русский](#русский)

---

## English

A Regular Expression Calculator for Windows built with WinAPI and Visual C++ 2022.

### Description

This application is a regular expression calculator that allows you to:

- Work with regular expressions in three modes: RegExp, Printf, Scanf
- Use different regex engines (Classic, Modern ECMAScript, Python-style, Google RE2)
- Save operation history
- Customize font size and window transparency
- Support DPI scaling

### Requirements

- Windows 10 or newer
- Visual Studio 2022 or newer
- Windows SDK 10.0

### Building

1. Open `RegExpCalc.sln` in Visual Studio 2022
2. Select configuration (Debug or Release) and platform (Win32 or x64)
3. Click Build -> Build Solution

### Usage

1. Run `RegExpCalc.exe`
2. Select working mode through the Mode menu
3. Enter data in the corresponding fields
4. The result will be displayed in the Result field

### Working Modes

#### RegExp Mode
- **Source**: Source string for searching
- **RegExp**: Regular expression pattern
- **Replace**: Replacement string

#### Printf Mode
- **Format**: Printf format string
- **Parameters**: Parameters for formatting

#### Scanf Mode
- **Format**: Scanf format string
- **Data**: Data to parse

### Hotkeys

- `Ctrl+Home`: Reset to default settings
- `Ctrl++`: Increase font size
- `Ctrl+-`: Decrease font size
- `Ctrl+Shift++`: Increase transparency
- `Ctrl+Shift+-`: Decrease transparency
- `F1`: Context-sensitive help

### Features

- Minimal executable file size
- No external dependencies
- DPI scaling support
- Settings saved in registry
- Operation history for each mode
- Compiled HTML Help (CHM) with comprehensive documentation
- Multiple regex engines with different feature sets

### Project Structure

- `main.cpp` - Application entry point
- `RegExpCalc.h/cpp` - Main application class
- `RegExpEngine.h/cpp` - Regex engines
- `RegExpClass.h/cpp` - Class for classic regex
- `regexp.c/h` - Classic regex engine source code
- `re2/` - Google RE2 regex library sources
- `Resource.h/RegExpCalc.rc` - Application resources
- `*.htm` - HTML Help topic files

### License

Copyright (C) 2024-2025

---

## Русский

Калькулятор регулярных выражений для Windows, созданный с использованием WinAPI и Visual C++ 2022.
## Описание

Это приложение является калькулятором регулярных выражений, который позволяет:

- Работать с регулярными выражениями в трех режимах: RegExp, Printf, Scanf
- Использовать различные движки регулярных выражений (Classic, Modern, Python)
- Сохранять историю операций
- Настраивать размер шрифта и прозрачность окна
- Поддерживать DPI scaling

## Требования
## Требования

- Windows 10 или новее
- Visual Studio 2022 или новее
- Windows SDK 10.0

## Сборка

1. Откройте файл `RegExpCalc.sln` в Visual Studio 2022
2. Выберите конфигурацию (Debug или Release) и платформу (x86 или x64)
3. Нажмите Build -> Build Solution

## Использование

1. Запустите `RegExpCalc.exe`
2. Выберите режим работы через меню Mode
3. Введите данные в соответствующие поля
4. Результат будет отображаться в поле Result

## Режимы работы

### RegExp режим
- **Source**: Исходная строка для поиска
- **RegExp**: Регулярное выражение
- **Replace**: Строка замены

### Printf режим
- **Format**: Форматная строка printf
- **Parameters**: Параметры для форматирования

### Scanf режим
- **Format**: Форматная строка scanf
- **Data**: Данные для парсинга

## Горячие клавиши

- `Ctrl+Home`: Сброс к настройкам по умолчанию
- `Ctrl++`: Увеличить размер шрифта
- `Ctrl+-`: Уменьшить размер шрифта
- `Ctrl+Shift++`: Увеличить прозрачность
- `Ctrl+Shift+-`: Уменьшить прозрачность

## Особенности

- Минимальный размер исполняемого файла
- Отсутствие внешних зависимостей
- Поддержка DPI scaling
- Сохранение настроек в реестре
- История операций для каждого режима

## Структура проекта

- `main.cpp` - Точка входа приложения
- `RegExpCalc.h/cpp` - Основной класс приложения
- `RegExpEngine.h/cpp` - Движки регулярных выражений
- `RegExpClass.h/cpp` - Класс для работы с классическими regex
- `regexp.c/h` - Исходный код классического движка regex
- `Resource.h/RegExpCalc.rc` - Ресурсы приложения

## Лицензия

Copyright (C) 2024



