/*
	LightBaker3000 Frontend project,
	(c) 2022 CrazyRussian
*/

#include "text_utils.h"
#include "common.h"

#include <string.h>

namespace TextUtils
{

AnsiStringList SplitTextSimple(const char* data, const size_t length, char splitter)
{
	AnsiStringList result;
	std::string line = "";

	size_t offset = 0;

	while (offset < length)
	{
		if (data[offset] == splitter)
		{
			if (!line.empty())
				result.push_back(line);
			line = "";
		}
		// Workarond for 2 symbols line break
		else if (splitter == '\n' && (offset + 1) < length && data[offset] == '\r')
		{
			if (data[offset] == '\r' && data[offset + 1] == '\n')
			{
				if (!line.empty())
					result.push_back(line);
				line = "";
				offset++;
			}
		}
		else
		{
			line += data[offset];
		}

		offset++;
	}

	if (line.size() > 0)
		result.push_back(line);

	return result;
}

AnsiStringList SplitTextWhitespaces(const char* data, const size_t length)
{
	AnsiStringList result;
	std::string line = "";

	size_t offset = 0;

	while (offset < length)
	{
		if (data[offset] < 33 && data[offset] > 0)
		{
			if (!line.empty())
				result.push_back(line);
			line = "";
		}
		else
			line += data[offset];

		offset++;
	}

	if (line.size() > 0)
		result.push_back(line);

	return result;
}


TextUtils::AnsiStringArray SplitTextEx(const char* data, size_t dataLength, AnsiTextSplitterList& splitters, char cEscapeChar)
{
	AnsiStringArray result;
	std::string line = "";

	size_t offset = 0;

	bool inString = false;

	while (offset < dataLength - 1)
	{
		bool atSplitter = false;
		size_t splitterSize = 0;

		// ������������� --->

		if (data[offset] == cEscapeChar)
		{
			if (offset <= dataLength - 2)
			{
				if (data[offset + 1] != cEscapeChar)
				{
					inString = !inString;
					offset++;
					continue;
				}
			}
			else
			{
				line += data[offset];
				offset++;
				continue;
			}
		}

		if (inString)
		{
			line += data[offset];
			offset++;
			continue;
		}

		// <--- �������������

		for (auto splitterItem : splitters)
		{
			if (splitterItem.length > 1)
			{
				if (!strncmp(splitterItem.value, &data[offset], splitterItem.length))
				{
					atSplitter = true;
					splitterSize = splitterItem.length;
					break;
				}
			}
			else
			{
				if (data[offset] == *splitterItem.value)
				{
					atSplitter = true;
					splitterSize = splitterItem.length;
					break;
				}
			}
		}

		if (atSplitter)
		{
			if (!line.empty())
				result.push_back(line);
			line = "";

			offset += splitterSize;
		}
		else
		{
			line += data[offset];
			offset++;
		}
	}

	if (line.size() > 0)
		result.push_back(line);


	return result;
}

void ParseCSVFile(const char* data, size_t dataSize, CSVSettings* settings, pfnCSVLoaderRowCallback rowCallback)
{
	AnsiStringArray rows = SplitTextEx(data, dataSize, settings->GetRowSplitters());

	int numColumns = -1;

	for (auto& row : rows)
	{
		AnsiStringArray cells = SplitTextEx(row.c_str(), row.length(), settings->GetColumnSplitters(), settings->StringEscapeChar());

		if (numColumns == -1)
			numColumns = cells.size();
		else
		{
			while (cells.size() <= numColumns)
				cells.push_back("");

			rowCallback(cells);
		}


	}

}

std::vector<std::string> Split(std::string s, std::string delimiter)
{
	size_t pos_start = 0, pos_end, delim_len = delimiter.length();
	std::string token;
	std::vector<std::string> res;

	while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos)
	{
		token = s.substr(pos_start, pos_end - pos_start);
		pos_start = pos_end + delim_len;
		res.push_back(token);
	}

	res.push_back(s.substr(pos_start));
	return res;
}

bool EmptyString(std::string& s)
{
	for (int i = 0; i < s.length(); i++)
		if (s[i] > 32)
			return false;

	return true;
}

std::vector<std::string> SpliteWhitespaces(std::string& s)
{
	std::string token;
	std::vector<std::string> res;

	const unsigned char* p = (const unsigned char*)s.c_str();

	while (*p)
	{
		if (*p < 33)
		{
			if (!EmptyString(token))
			{
				res.push_back(token);
				token = "";
			}
		}
		else
		{
			token += *p;
		}

		p++;
	}

	if (!EmptyString(token))
	{
		res.push_back(token);
	}

	return res;
}

void ReplaceAll(std::string& s, std::string const& toReplace, std::string const& replaceWith)
{
	std::string buf;
	std::size_t pos = 0;
	std::size_t prevPos;

	// Reserves rough estimate of final size of string.
	buf.reserve(s.size());

	while (true) {
		prevPos = pos;
		pos = s.find(toReplace, pos);
		if (pos == std::string::npos)
			break;
		buf.append(s, prevPos, pos - prevPos);
		buf += replaceWith;
		pos += toReplace.size();
	}

	buf.append(s, prevPos, s.size() - prevPos);
	s.swap(buf);
}

CSVSettings::CSVSettings()
{
	m_lstRowSplitters.clear();
	m_lstColumnSplitters.clear();
	m_cEscapeChar = '"';
}

CSVSettings::~CSVSettings()
{
	m_lstColumnSplitters.clear();
	m_lstRowSplitters.clear();
}

void CSVSettings::InitDefault()
{
	AddColumnSplitter(";");
	AddColumnSplitter("\t");
	AddRowSplitter("\n");
}

void CSVSettings::AddRowSplitter(const char* splitter)
{
	textSplitter_t record;
	record.value = splitter;
	record.length = strlen(splitter);

	m_lstRowSplitters.push_back(record);
}

void CSVSettings::AddColumnSplitter(const char* splitter)
{
	textSplitter_t record;
	record.value = splitter;
	record.length = strlen(splitter);

	m_lstColumnSplitters.push_back(record);
}

void CSVSettings::SetStringEscaping(char escapeChar)
{
	m_cEscapeChar = escapeChar;
}

AnsiTextSplitterList& CSVSettings::GetRowSplitters()
{
	return m_lstRowSplitters;
}

TextUtils::AnsiTextSplitterList& CSVSettings::GetColumnSplitters()
{
	return m_lstColumnSplitters;
}

char CSVSettings::StringEscapeChar()
{
	return m_cEscapeChar;
}

} // namespace TextUtils

// Cheers to G-Cont!
const char *VA(const char *format, ...)
{
    va_list     argptr;
    static char string[16][128], *s;
    static int  stringindex = 0;

    s           = string[stringindex];
    stringindex = (stringindex + 1) & 15;
    va_start(argptr, format);
    vsnprintf(s, 128, format, argptr);
    va_end(argptr);
    return s;
}

size_t strlcpy(char *dst, const char *src, size_t maxlen)
{
    const size_t srclen = strlen(src);
    if (srclen + 1 < maxlen)
    {
        memcpy(dst, src, srclen + 1);
    }
    else if (maxlen != 0)
    {
        memcpy(dst, src, maxlen - 1);
        dst[maxlen - 1] = '\0';
    }
    return srclen;
}

size_t StrHash(const char *val)
{
    std::string_view v(val);
    return std::hash<std::string_view>{}(v);
}

