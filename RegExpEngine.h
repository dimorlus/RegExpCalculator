#pragma once

#include "pch.h"
#include "RegExpClass.h"
#include <regex>

// Результат выполнения RegExp операции
struct RegExpResult
{
    std::string result;           // Основной результат
    std::string groups;           // Найденные группы (\0, \1, \2, ...) как строка
    bool success;                 // Успешность операции
    std::string error;            // Сообщение об ошибке
};

// Базовый класс для RegExp движков
class RegExpEngine
{
public:
    virtual ~RegExpEngine() = default;
    virtual RegExpResult Execute(const std::string& source, const std::string& pattern, const std::string& replacement) = 0;
    virtual std::string GetName() const = 0;
};

// Classic RegExp движок (на основе regexp.c)
class ClassicRegExpEngine : public RegExpEngine
{
public:
    RegExpResult Execute(const std::string& source, const std::string& pattern, const std::string& replacement) override;
    std::string GetName() const override { return "Classic"; }
};

// Modern RegExp движок (на основе std::regex)
class ModernRegExpEngine : public RegExpEngine
{
public:
    RegExpResult Execute(const std::string& source, const std::string& pattern, const std::string& replacement) override;
    std::string GetName() const override { return "Modern"; }
};

// Python RegExp движок (имитация Python regex)
class PythonRegExpEngine : public RegExpEngine
{
public:
    RegExpResult Execute(const std::string& source, const std::string& pattern, const std::string& replacement) override;
    std::string GetName() const override { return "Python"; }
    
private:
    std::string ConvertPythonPattern(const std::string& pattern);
    std::string ConvertPythonReplacement(const std::string& replacement);
};

// RE2 движок (Google RE2 - линейная сложность, защита от ReDoS)
class RE2Engine : public RegExpEngine
{
public:
    RegExpResult Execute(const std::string& source, const std::string& pattern, const std::string& replacement) override;
    std::string GetName() const override { return "RE2 (Google)"; }
};
