#include "pch.h"
#include "RegExpEngine.h"
#include <regex>
#include <sstream>
#include <iomanip>

// Constants for Classic RegExp output format (like rxcc)
const char* const ttls[] =
{ "Res:", "&:", "\\0:", "\\1:", "\\2:", "\\3:",
 "\\4:", "\\5:", "\\6:", "\\7:", "\\8:", "\\9:",
 "\\A:", "\\B:", "\\C:", "\\D:", "\\E:", "\\F:" };

const char* const rrpls[] =
{ "\0", "&", "\\0", "\\1", "\\2", "\\3",
 "\\4", "\\5", "\\6", "\\7", "\\8", "\\9",
 "\\A", "\\B", "\\C", "\\D", "\\E", "\\F" };

#define _RLINES_ sizeof(ttls)/sizeof(ttls[0])

// Classic RegExp Engine Implementation
RegExpResult ClassicRegExpEngine::Execute(const std::string& source, const std::string& pattern, const std::string& replacement)
{
    RegExpResult result;
    result.success = false;
    
    try
    {
        char srcBuf[4096];
        char patBuf[4096];
        char repBuf[4096];
        
        strncpy_s(srcBuf, sizeof(srcBuf), source.c_str(), _TRUNCATE);
        strncpy_s(patBuf, sizeof(patBuf), pattern.c_str(), _TRUNCATE);
        strncpy_s(repBuf, sizeof(repBuf), replacement.c_str(), _TRUNCATE);
        
        TRegExp regex(patBuf);
        
        if (regex.RegError() != RE_OK)
        {
            result.error = std::string(regex.regerr());
            return result;
        }
        
        if (regex.Match(srcBuf))
        {
            // Форматируем вывод в стиле rxcc: все замены построчно
            char resultBuf[16384] = { '\0' };
            char* presult = resultBuf;
            int remaining = sizeof(resultBuf);
            
            for (int i = 0; i < _RLINES_; i++)
            {
                char str[4096] = { '\0' };
                if (rrpls[i][0] == '\0')
                    regex.Replace(repBuf, str, sizeof(str));
                else
                    regex.Replace(rrpls[i], str, sizeof(str));
                
                int written = snprintf(presult, remaining, "% 4s %s\r\n", ttls[i], str);
                if (written > 0 && written < remaining)
                {
                    presult += written;
                    remaining -= written;
                }
            }
            
            result.result = std::string(resultBuf);
            result.success = true;
        }
        else
        {
            result.error = "No match found";
        }
    }
    catch (const std::exception& e)
    {
        result.error = "Exception: " + std::string(e.what());
    }
    
    return result;
}

// Modern RegExp Engine Implementation
RegExpResult ModernRegExpEngine::Execute(const std::string& source, const std::string& pattern, const std::string& replacement)
{
    RegExpResult result;
    result.success = false;
    
    try
    {
        std::regex regex(pattern, std::regex_constants::ECMAScript);
        std::smatch match;
        
        if (std::regex_search(source, match, regex))
        {
            // Форматируем вывод в стиле rxcc
            char resultBuf[16384] = { '\0' };
            char* presult = resultBuf;
            int remaining = sizeof(resultBuf);
            
            std::string fullMatch = match[0].str();
            
            // Res: - замена только совпадения (используя ECMAScript format_default)
            // std::regex_replace использует $& для полного совпадения, $1-$9 для групп
            std::string replaced_match = std::regex_replace(fullMatch, regex, replacement);
            int written = snprintf(presult, remaining, "% 4s %s\r\n", "Res:", replaced_match.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // $&: - полное совпадение (ECMAScript стандарт)
            written = snprintf(presult, remaining, "% 4s %s\r\n", "$&:", fullMatch.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // Группы $0-$9 (ECMAScript стандарт: $0 = полное совпадение, $1-$9 = группы)
            for (size_t i = 0; i <= 9; i++)
            {
                char label[8];
                snprintf(label, sizeof(label), "$%d:", (int)i);
                std::string groupValue = (i < match.size() && match[i].matched) ? match[i].str() : "";
                written = snprintf(presult, remaining, "% 4s %s\r\n", label, groupValue.c_str());
                if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            }
            
            result.result = std::string(resultBuf);
            result.success = true;
        }
        else
        {
            result.error = "No match found";
        }
    }
    catch (const std::regex_error& e)
    {
        result.error = "Regex error: " + std::string(e.what());
    }
    catch (const std::exception& e)
    {
        result.error = "Exception: " + std::string(e.what());
    }
    
    return result;
}

// Python RegExp Engine Implementation
RegExpResult PythonRegExpEngine::Execute(const std::string& source, const std::string& pattern, const std::string& replacement)
{
    RegExpResult result;
    result.success = false;
    
    try
    {
        // Конвертируем Python паттерн в ECMAScript
        std::string convertedPattern = ConvertPythonPattern(pattern);
        
        std::regex regex(convertedPattern, std::regex_constants::ECMAScript);
        std::smatch match;
        
        if (std::regex_search(source, match, regex))
        {
            // Форматируем вывод в стиле rxcc
            char resultBuf[16384] = { '\0' };
            char* presult = resultBuf;
            int remaining = sizeof(resultBuf);
            
            std::string fullMatch = match[0].str();
            
            // Res: - замена только совпадения
            // Python использует \0 для полного совпадения, \1-\9 для групп
            // Но std::regex_replace использует ECMAScript формат ($&, $1-$9)
            // Поэтому конвертируем Python замену в ECMAScript формат
            std::string ecma_replacement = ConvertPythonReplacement(replacement);
            std::string replaced_match = std::regex_replace(fullMatch, regex, ecma_replacement);
            int written = snprintf(presult, remaining, "% 4s %s\r\n", "Res:", replaced_match.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // \0: - полное совпадение (Python стандарт)
            written = snprintf(presult, remaining, "% 4s %s\r\n", "\\0:", fullMatch.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // Группы \1-\9 (Python стандарт)
            for (size_t i = 1; i <= 9; i++)
            {
                char label[8];
                snprintf(label, sizeof(label), "\\%d:", (int)i);
                std::string groupValue = (i < match.size() && match[i].matched) ? match[i].str() : "";
                written = snprintf(presult, remaining, "% 4s %s\r\n", label, groupValue.c_str());
                if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            }
            
            result.result = std::string(resultBuf);
            result.success = true;
        }
        else
        {
            result.error = "No match found";
        }
    }
    catch (const std::regex_error& e)
    {
        result.error = "Regex error: " + std::string(e.what());
    }
    catch (const std::exception& e)
    {
        result.error = "Exception: " + std::string(e.what());
    }
    
    return result;
}

std::string PythonRegExpEngine::ConvertPythonPattern(const std::string& pattern)
{
    // Простая конвертация Python regex в ECMAScript
    // В реальном приложении здесь должна быть более сложная логика
    std::string converted = pattern;
    
    // Заменяем Python-специфичные конструкции на ECMAScript эквиваленты
    // Это упрощенная версия, в реальности нужна более полная конвертация
    
    return converted;
}

std::string PythonRegExpEngine::ConvertPythonReplacement(const std::string& replacement)
{
    // Конвертируем Python формат замены (\0, \1-\9) в ECMAScript ($&, $1-$9)
    std::string result = replacement;
    size_t pos = 0;
    
    while (pos < result.length())
    {
        if (result[pos] == '\\' && pos + 1 < result.length())
        {
            char next = result[pos + 1];
            if (next == '0')
            {
                // \0 -> $& (полное совпадение)
                result.replace(pos, 2, "$&");
                pos += 2;
            }
            else if (next >= '1' && next <= '9')
            {
                // \1-\9 -> $1-$9
                result[pos] = '$';
                pos += 2;
            }
            else
            {
                pos++;
            }
        }
        else
        {
            pos++;
        }
    }
    
    return result;
}

// RE2 Engine Implementation
#include "re2/re2.h"

RegExpResult RE2Engine::Execute(const std::string& source, const std::string& pattern, const std::string& replacement)
{
    RegExpResult result;
    result.success = false;
    
    try
    {
        // Создаем RE2 объект
        RE2 regex(pattern);
        
        if (!regex.ok())
        {
            result.error = "Invalid regular expression: " + regex.error();
            return result;
        }
        
        // Извлекаем первое совпадение и его группы
        int num_groups = regex.NumberOfCapturingGroups();
        
        // Используем простой подход: извлекаем только то количество групп, которое есть в паттерне
        if (num_groups == 0)
        {
            // Нет групп - только полное совпадение
            std::string fullMatch;
            re2::StringPiece input(source);
            if (!RE2::FindAndConsume(&input, regex, &fullMatch))
            {
                result.error = "No match found";
                return result;
            }
            
            // Для замены используем временную строку с источником
            std::string temp = source;
            std::string replaced = replacement;
            // RE2 использует \0 или \& для полного совпадения
            size_t pos = 0;
            while ((pos = replaced.find("\\0", pos)) != std::string::npos) {
                replaced.replace(pos, 2, fullMatch);
                pos += fullMatch.length();
            }
            
            char resultBuf[16384] = { '\0' };
            char* presult = resultBuf;
            int remaining = sizeof(resultBuf);
            
            int written = snprintf(presult, remaining, "% 4s %s\r\n", "Res:", replaced.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            written = snprintf(presult, remaining, "% 4s %s\r\n", "\\0:", fullMatch.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            for (int i = 1; i <= 9; i++)
            {
                char label[8];
                snprintf(label, sizeof(label), "\\%d:", i);
                written = snprintf(presult, remaining, "% 4s %s\r\n", label, "");
                if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            }
            
            result.result = std::string(resultBuf);
            result.success = true;
            return result;
        }
        
        // Есть группы - извлекаем их
        std::vector<std::string> groups(num_groups);
        std::vector<RE2::Arg> argv(num_groups);
        std::vector<RE2::Arg*> args(num_groups);
        
        for (int i = 0; i < num_groups; i++)
        {
            argv[i] = &groups[i];
            args[i] = &argv[i];
        }
        
        // Извлекаем группы (без группы 0 - полного совпадения)
        if (RE2::PartialMatchN(source, regex, args.data(), num_groups))
        {
            // Нужно также извлечь полное совпадение отдельно
            std::string fullMatch;
            RE2::PartialMatch(source, regex, &fullMatch);
            
            // Выполняем замену вручную, подставляя группы
            std::string replaced = replacement;
            
            // Заменяем \0 на полное совпадение
            size_t pos = 0;
            while ((pos = replaced.find("\\0", pos)) != std::string::npos) {
                replaced.replace(pos, 2, fullMatch);
                pos += fullMatch.length();
            }
            
            // Заменяем \1-\9 на соответствующие группы
            for (int i = 1; i <= 9 && i <= num_groups; i++) {
                std::string pattern_str = "\\" + std::to_string(i);
                pos = 0;
                while ((pos = replaced.find(pattern_str, pos)) != std::string::npos) {
                    replaced.replace(pos, pattern_str.length(), groups[i - 1]);
                    pos += groups[i - 1].length();
                }
            }
            
            // Форматируем вывод в стиле rxcc
            char resultBuf[16384] = { '\0' };
            char* presult = resultBuf;
            int remaining = sizeof(resultBuf);
            
            // Res: - результат замены
            int written = snprintf(presult, remaining, "% 4s %s\r\n", "Res:", replaced.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // \0: - полное совпадение
            written = snprintf(presult, remaining, "% 4s %s\r\n", "\\0:", fullMatch.c_str());
            if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            
            // Группы \1-\9
            for (int i = 1; i <= 9; i++)
            {
                char label[8];
                snprintf(label, sizeof(label), "\\%d:", i);
                const char* value = (i - 1 < (int)groups.size()) ? groups[i - 1].c_str() : "";
                written = snprintf(presult, remaining, "% 4s %s\r\n", label, value);
                if (written > 0 && written < remaining) { presult += written; remaining -= written; }
            }
            
            result.result = std::string(resultBuf);
            result.success = true;
        }
        else
        {
            result.error = "No match found";
        }
    }
    catch (const std::exception& e)
    {
        result.error = "RE2 error: " + std::string(e.what());
    }
    
    return result;
}
