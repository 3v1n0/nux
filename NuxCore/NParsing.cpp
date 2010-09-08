/*
 * Copyright 2010 Inalogic Inc.
 *
 * This program is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU Lesser General Public License version 3, as
 * published by the  Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranties of 
 * MERCHANTABILITY, SATISFACTORY QUALITY or FITNESS FOR A PARTICULAR 
 * PURPOSE.  See the applicable version of the GNU Lesser General Public 
 * License for more details.
 * 
 * You should have received a copy of both the GNU Lesser General Public 
 * License version 3 along with this program.  If not, see 
 * <http://www.gnu.org/licenses/>
 *
 * Authored by: Jay Taoko <jay.taoko_AT_gmail_DOT_com>
 *
 */


#include "NKernel.h" 
#include "NParsing.h"

#define CHAR_TAB    9   // '"'
#define CHAR_QUOTE  34  // '"'

NAMESPACE_BEGIN

//
// Get a string from a text string.
//
BOOL ParseTCHAR
(
	const TCHAR* Stream, 
	const TCHAR* Match,
	TCHAR*		 Value,
    INT          Size,
	INT			 MaxLen
)
{
	const TCHAR* Found = Strfind(Stream, Match);
	const TCHAR* Start;

	if( Found )
	{
		Start = Found + StringLength(Match);
		if( *Start == '\x22' ) // Character '"'
		{
			// The value begins with the quotation mark character: ". We skip it.
			Strncpy( Value, Size, Start+1, MaxLen );
			Value[MaxLen-1]=0;
			TCHAR* Temp = Strstr( Value, TEXT("\x22") );
			if( Temp != NULL )
            {
                // We read in the termination quotation mark. Set it t0 0 to null terminate the Value buffer.
                *Temp=0;
            }
		}
		else
		{
			// Non-quoted string without spaces.
			Strncpy( Value, Size, Start, MaxLen );
			Value[MaxLen-1]=0;
			TCHAR* Temp;
			Temp = Strstr( Value, TEXT(" ")  ); if( Temp ) *Temp=0;
			Temp = Strstr( Value, TEXT("\r") ); if( Temp ) *Temp=0;
			Temp = Strstr( Value, TEXT("\n") ); if( Temp ) *Temp=0;
			Temp = Strstr( Value, TEXT("\t") ); if( Temp ) *Temp=0;
			Temp = Strstr( Value, TEXT(",")  ); if( Temp ) *Temp=0;
		}
		return 1;
	}
	else
        return 0;
}

//
// Checks if a command-line parameter exists in the stream.
//
BOOL ParseParam( const TCHAR* Stream, const TCHAR* Param )
{
	const TCHAR* Start = Stream;
	if( *Stream )
	{
		while( (Start=Strfind(Start+1,Param)) != NULL )
		{
			if( Start>Stream && (Start[-1]=='-' || Start[-1]=='/') )
			{
				const TCHAR* End = Start + StringLength(Param);
				if ( End == NULL || *End == 0 || IsWhitespaceChar(*End) )
                    return 1;
			}
		}
	}
	return 0;
}

// 
// Parse a string.
//
BOOL ParseFString( const TCHAR* Stream, const TCHAR* Match, NString& Value )
{
	TCHAR Temp[4096]=TEXT("");
	if( ParseTCHAR( Stream, Match, Temp, NUX_ARRAY_COUNT(Temp), NUX_ARRAY_COUNT(Temp) ) )
	{
		Value = Temp;
		return 1;
	}
	else return 0;
}

//
// Parse a quadword.
//
BOOL ParseQWORD( const TCHAR* Stream, const TCHAR* Match, QWORD& Value )
{
	return ParseSQWORD( Stream, Match, *(SQWORD*)&Value );
}

//
// Parse a signed quadword.
//
BOOL ParseSQWORD( const TCHAR* Stream, const TCHAR* Match, SQWORD& Value )
{
	TCHAR Temp[4096]=TEXT(""), *Ptr=Temp;
	if( ParseTCHAR( Stream, Match, Temp, NUX_ARRAY_COUNT(Temp), NUX_ARRAY_COUNT(Temp) ) )
	{
		Value = 0;
		BOOL Negative = (*Ptr=='-');
		Ptr += Negative;
		while( *Ptr>='0' && *Ptr<='9' )
			Value = Value*10 + *Ptr++ - '0';
		if( Negative )
			Value = -Value;
		return 1;
	}
	else return 0;
}

//
// Get a DWORD.
//
BOOL ParseDWORD( const TCHAR* Stream, const TCHAR* Match, DWORD& Value )
{
	const TCHAR* Temp = Strfind(Stream, Match);
	TCHAR* End;
	if( Temp==NULL )
		return 0;
	Value = Strtoi( Temp + StringLength(Match), &End, 10 );

	return 1;
}

//
// Get a byte.
//
BOOL ParseBYTE( const TCHAR* Stream, const TCHAR* Match, BYTE& Value )
{
	const TCHAR* Temp = Strfind(Stream,Match);
	if( Temp==NULL )
		return 0;
	Temp += StringLength( Match );
	Value = (BYTE)CharToInteger( Temp );
	return Value!=0 || IsDigitChar(Temp[0]);
}

//
// Get a signed byte.
//
BOOL ParseSBYTE( const TCHAR* Stream, const TCHAR* Match, SBYTE& Value )
{
	const TCHAR* Temp = Strfind(Stream,Match);
	if( Temp==NULL )
		return 0;
	Temp += StringLength( Match );
    Value = CharToInteger( Temp );
	return Value!=0 || IsDigitChar(Temp[0]);
}

//
// Get a word.
//
BOOL ParseWORD( const TCHAR* Stream, const TCHAR* Match, WORD& Value )
{
	const TCHAR* Temp = Strfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Temp += StringLength( Match );
	Value = (WORD)CharToInteger( Temp );
	return Value!=0 || IsDigitChar(Temp[0]);
}

//
// Get a signed word.
//
BOOL ParseSWORD( const TCHAR* Stream, const TCHAR* Match, SWORD& Value )
{
	const TCHAR* Temp = Strfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Temp += StringLength( Match );
	Value = (SWORD)CharToInteger( Temp );
	return Value!=0 || IsDigitChar(Temp[0]);
}

//
// Get a floating-point number.
//
BOOL ParseFLOAT( const TCHAR* Stream, const TCHAR* Match, float& Value )
{
	const TCHAR* Temp = Strfind( Stream, Match );
	if( Temp==NULL )
		return 0;
    Value = CharToDouble(Temp+StringLength(Match));
	return 1;
}

//
// Get a signed double word.
//
BOOL ParseINT( const TCHAR* Stream, const TCHAR* Match, INT& Value )
{
	const TCHAR* Temp = Strfind( Stream, Match );
	if( Temp==NULL )
		return 0;
	Value = CharToInteger( Temp + StringLength(Match) );
	return 1;
}

//
// Get a boolean value.
//
BOOL ParseUBOOL( const TCHAR* Stream, const TCHAR* Match, BOOL& OnOff )
{
	TCHAR TempStr[16];
	if( ParseTCHAR( Stream, Match, TempStr, NUX_ARRAY_COUNT(TempStr), NUX_ARRAY_COUNT(TempStr)-1 ) )
	{
		OnOff
		=	!Stricmp(TempStr,TEXT("On"))
		||	!Stricmp(TempStr,TEXT("True"))
		||	!Stricmp(TempStr,TEXT("1"));
		return 1;
	}
	else
        return 0;
}

//
// Sees if Stream starts with the named command.  If it does,
// skips through the command and blanks past it.  Returns 1 of match,
// 0 if not.
//
BOOL ParseCommand
(
	const TCHAR** Stream, 
	const TCHAR*  Match
)
{
	while( (**Stream==' ')||(**Stream==9) )
		(*Stream)++;

	if( TCharStringNICompare(*Stream,Match,StringLength(Match))==0 )
	{
		*Stream += StringLength(Match);
		if( !IsAlphanumericChar(**Stream) )
		{
			while ((**Stream==' ')||(**Stream==9)) (*Stream)++;
			return 1; // Success.
		}
		else
		{
			*Stream -= StringLength(Match);
			return 0; // Only found partial match.
		}
	}
	else return 0; // No match.
}

//
// Get next command.  Skips past comments and cr's.
//
void ParseNext( const TCHAR** Stream )
{
	// Skip over spaces, tabs, cr's, and linefeeds.
	SkipJunk:
	while( **Stream==' ' || **Stream==9 || **Stream==13 || **Stream==10 )
		++*Stream;

	if( **Stream==';' )
	{
		// Skip past comments.
		while( **Stream!=0 && **Stream!=10 && **Stream!=13 )
			++*Stream;
		goto SkipJunk;
	}

	// Upon exit, *Stream either points to valid Stream or a nul.
}

//
// Grab the next space-delimited string from the input stream.
// If quoted, gets entire quoted string.
//
BOOL ParseToken( const TCHAR*& Str, TCHAR* Result, INT MaxLen, BOOL UseEscape )
{
	INT Len=0;

	// Skip spaces and tabs.
	while( *Str==' ' || *Str==9 )
		Str++;
	if( *Str == 34 )
	{
		// Get quoted string.
		Str++;
		while( *Str && *Str!=34 && (Len+1)<MaxLen )
		{
			TCHAR c = *Str++;
			if( c=='\\' && UseEscape )
			{
				// Get escape.
				c = *Str++;
				if( !c )
					break;
			}
			if( (Len+1)<MaxLen )
				Result[Len++] = c;
		}
		if( *Str==34 )
			Str++;
	}
	else
	{
		// Get unquoted string.
		for( ; *Str && *Str!=' ' && *Str!=9; Str++ )
			if( (Len+1)<MaxLen )
				Result[Len++] = *Str;
	}
	Result[Len]=0;
	return Len!=0;
}
BOOL ParseToken( const TCHAR*& Str, NString& Arg, BOOL UseEscape )
{
	Arg.Clear();

	// Skip spaces and tabs.
	while( IsWhitespaceChar(*Str) )
		Str++;

	if ( *Str == TEXT('"') )
	{
		// Get quoted string.
		Str++;
		while( *Str && *Str != TEXT('"') )
		{
			TCHAR c = *Str++;
			if( c == TEXT('\\') && UseEscape )
			{
				// Get escape.
				c = *Str++;
				if( !c )
					break;
			}

			Arg += c;
		}

		if ( *Str == TEXT('"') )
			Str++;
	}
	else
	{
		// Get unquoted string.
		for( ; *Str && !IsWhitespaceChar(*Str); Str++ )
		{
			Arg += *Str;
		}
	}

	return Arg.Length() > 0;
}
NString ParseToken( const TCHAR*& Str, BOOL UseEscape )
{
	TCHAR Buffer[1024];
	if( ParseToken( Str, Buffer, NUX_ARRAY_COUNT(Buffer), UseEscape ) )
		return Buffer;
	else
		return TEXT("");
}

//
// Get a line of Stream (everything up to, but not including, CR/LF.
// Returns 0 if ok, nonzero if at end of stream and returned 0-length string.
//
BOOL ParseLine
(
	const TCHAR**	Stream,
	TCHAR*			Result,
	INT				MaxLen,
	BOOL			Exact
)
{
	BOOL GotStream=0;
	BOOL IsQuoted=0;
	BOOL Ignore=0;

	*Result=0;
	while( **Stream!=0 && **Stream!=10 && **Stream!=13 && --MaxLen>0 )
	{
		// Start of comments.
		if( !IsQuoted && !Exact && (*Stream)[0]=='/' && (*Stream)[1]=='/' )
			Ignore = 1;
		
		// Command chaining.
		if( !IsQuoted && !Exact && **Stream=='|' )
			break;

		// Check quoting.
		IsQuoted = IsQuoted ^ (**Stream==34);
		GotStream=1;

		// Got stuff.
		if( !Ignore )
			*(Result++) = *((*Stream)++);
		else
			(*Stream)++;
	}
	if( Exact )
	{
		// Eat up exactly one CR/LF.
		if( **Stream == 13 )
			(*Stream)++;
		if( **Stream == 10 )
			(*Stream)++;
	}
	else
	{
		// Eat up all CR/LF's.
		while( **Stream==10 || **Stream==13 || **Stream=='|' )
			(*Stream)++;
	}
	*Result=0;
	return **Stream!=0 || GotStream;
}
BOOL ParseLine
(
	const TCHAR**	Stream,
	NString&		Result,
	BOOL			Exact
)
{
	BOOL GotStream=0;
	BOOL IsQuoted=0;
	BOOL Ignore=0;

	Result = TEXT("");

	while( **Stream!=0 && **Stream!=10 && **Stream!=13 )
	{
		// Start of comments.
		if( !IsQuoted && !Exact && (*Stream)[0]=='/' && (*Stream)[1]=='/' )
			Ignore = 1;

		// Command chaining.
		if( !IsQuoted && !Exact && **Stream=='|' )
			break;

		// Check quoting.
		IsQuoted = IsQuoted ^ (**Stream==34);
		GotStream=1;

		// Got stuff.
		if( !Ignore )
		{
			Result += ( *((*Stream)++) );
		}
		else
		{
			(*Stream)++;
		}
	}
	if( Exact )
	{
		// Eat up exactly one CR/LF.
		if( **Stream == 13 )
			(*Stream)++;
		if( **Stream == 10 )
			(*Stream)++;
	}
	else
	{
		// Eat up all CR/LF's.
		while( **Stream==10 || **Stream==13 || **Stream=='|' )
			(*Stream)++;
	}

	return **Stream!=0 || GotStream;
}

NAMESPACE_END
