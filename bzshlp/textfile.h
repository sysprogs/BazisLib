#pragma once
#include "cmndef.h"
#include "../bzscore/autofile.h"
#include "../bzscore/buffer.h"
#include "../bzscore/string.h"
#include "../bzscore/platform.h"

namespace BazisLib
{
	namespace _Core
	{
		template <class _CharType> static inline _CharType *_FindSubstring(_CharType *String, _CharType *Substring)
		{
			ASSERT(FALSE);
			return NULL;
		}

		template<> static inline char *_FindSubstring<char>(char *String, char *Substring)
		{
			return strstr(String, Substring);
		}

		template<> static inline wchar_t *_FindSubstring<wchar_t>(wchar_t *String, wchar_t *Substring)
		{
			return wcsstr(String, Substring);
		}

		template <class _CharType, unsigned _LineEnding, unsigned _LineEndingCharCount> class _TextFile : AUTO_OBJECT
		{
		private:
			BazisLib::CBuffer m_Buffer;
			unsigned m_PendingLine;
			bool m_bEOF;

			enum {ReadChunkSize = 50};

			DECLARE_REFERENCE(BazisLib::AIFile, m_pFile);
		
		public:

			_TextFile(const ManagedPointer<BazisLib::AIFile> &pFile) :
				INIT_REFERENCE(m_pFile, pFile),
				m_PendingLine(0),
				m_bEOF(false)
			{
				if (m_pFile && (sizeof(_CharType) == sizeof(wchar_t)))
				{
					m_Buffer.EnsureSize(sizeof(wchar_t));
					if (m_pFile->Read(m_Buffer.GetData(), sizeof(wchar_t)) == sizeof(wchar_t))
					{
						if (*((wchar_t *)m_Buffer.GetData()) != 0xFEFF)
							m_Buffer.SetSize(sizeof(wchar_t));
					}
				}
			}

			bool IsEOF()
			{
				return m_bEOF && !m_Buffer.GetSize();
			}

			bool HasMoreData()
			{
				return !IsEOF();
			}

			bool Valid()
			{
				return m_pFile && m_pFile->Valid();
			}

			_DynamicStringT<_CharType> ReadLine()
			{
				if (!m_pFile)
					return _DynamicStringT<_CharType>();
				for (;;)
				{
					if (m_Buffer.GetSize())
					{
						unsigned LineEnding[2] = {_LineEnding, 0};
						_CharType *pEnd = _FindSubstring((_CharType *)m_Buffer.GetData(), (_CharType *)LineEnding);
						if (pEnd)
						{
							pEnd[0] = 0;
							pEnd += _LineEndingCharCount;
							_DynamicStringT<_CharType> str((_CharType *)m_Buffer.GetData());
							unsigned dif = (unsigned)(pEnd - (_CharType *)m_Buffer.GetData());
							memmove(m_Buffer.GetData(), pEnd, m_Buffer.GetSize() - (dif - 1) * sizeof(_CharType));	//Copy one null-terminating character
							m_Buffer.SetSize(m_Buffer.GetSize() - dif * sizeof(_CharType));
							return str;
						}
					}
					m_Buffer.EnsureSize(m_Buffer.GetSize() + (ReadChunkSize + 1) * sizeof(_CharType));
					size_t done = m_pFile->Read(((char *)m_Buffer.GetData()) + m_Buffer.GetSize(), ReadChunkSize * sizeof(_CharType));
					if (!done)
					{
						m_bEOF = true;
						_DynamicStringT<_CharType> ret = ((_CharType *)m_Buffer.GetData());
						m_Buffer.SetSize(0);
						return ret;
					}
					m_Buffer.SetSize(m_Buffer.GetSize() + done);
					((_CharType *)m_Buffer.GetData())[m_Buffer.GetSize() / sizeof(_CharType)] = 0;
				}
			}
		};
	}

	typedef _Core::_TextFile<char, LINE_ENDING_SEQUENCE_A, LINE_ENDING_SEQUENCE_LEN> TextANSIFileReader;
	typedef _Core::_TextFile<wchar_t, LINE_ENDING_SEQUENCE_W, LINE_ENDING_SEQUENCE_LEN> TextUnicodeFileReader;

	typedef _Core::_TextFile<char, '\n', 1> TextANSIUNIXFileReader;
	typedef _Core::_TextFile<wchar_t, '\n\0', 1> TextUnicodeUNIXFileReader;
}
