/*
	LightBaker3000 Frontend project,
	(c) 2022-2025 CrazyRussian
*/

#pragma once

#include <string>
#include <list>
#include <functional>
#include <vector>

namespace TextUtils
{

	typedef struct textSplitter_s
	{
		const char* value;
		size_t	length;
	}textSplitter_t;

	typedef std::list<textSplitter_t> AnsiTextSplitterList;

	typedef std::list<const char*> AnsiConstCharlist;
	typedef std::list<std::string> AnsiStringList;
	typedef std::vector<std::string> AnsiStringArray;

	typedef std::function<void(TextUtils::AnsiStringArray&)> pfnCSVLoaderRowCallback;

	class CSVSettings
	{
		AnsiTextSplitterList m_lstRowSplitters;
		AnsiTextSplitterList m_lstColumnSplitters;

		char m_cEscapeChar;
	public:
		CSVSettings();
		~CSVSettings();

		void InitDefault();

		void AddRowSplitter(const char* splitter);
		void AddColumnSplitter(const char* splitter);

		void SetStringEscaping(char escapeChar);

		AnsiTextSplitterList& GetRowSplitters();
		AnsiTextSplitterList& GetColumnSplitters();
		char StringEscapeChar();
	};

	AnsiStringList SplitTextWhitespaces(const char* data, const size_t length);
	AnsiStringList SplitTextSimple(const char* data, const size_t length, char splitter);
	AnsiStringArray SplitTextEx(const char* data, size_t dataLength, AnsiTextSplitterList& splitters, char cEscapeChar = 0);

	void ParseCSVFile(const char* data, size_t dataSize, CSVSettings* settings, pfnCSVLoaderRowCallback rowCallback);

	// for string delimiter
	std::vector<std::string> Split(std::string s, std::string delimiter);
	bool EmptyString(std::string& s);

	std::vector<std::string> SpliteWhitespaces(std::string& s);


	void ReplaceAll(
		std::string& s,
		std::string const& toReplace,
		std::string const& replaceWith
	);

}

const char *VA(const char *format, ...);

#undef strlcpy
size_t strlcpy(char *dst, const char *src, size_t maxlen);


size_t StrHash(const char* val);