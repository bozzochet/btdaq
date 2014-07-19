//  $Id: astring.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __ASTRING___
#define __ASTRING___



//  AMS AString (AString)

#include <iostream.h>


extern "C" {
#include <string.h>
}



class AString{

//
//	A AString is a sequence of characters of type char.  The sequence of
//	characters is stored as a null-terminated array (ASCIZ format).
//	Therefore, a AString may NOT contain a null character.  More than one
//	AString may share the same data (string representation structure).
//	Garbage collection is done with a reference count in the string
//	representation.
//--

protected:
 class	 Data{		// AString's representation
 public:
 char *pString ;        // Pointer to the data
 unsigned int   RefC;            // Reference count
 Data();
 Data(const char *);
 Data(char c, int n=0);
 Data(const Data &);
 ~Data();
 
 };

 Data *  pData;	// Pointer to AString representation
 static Data* defaultData();
 void _mutator();
public:
    AString(): pData(defaultData()){}
    AString(const AString & s):pData(s.pData){(pData->RefC)++;}
    AString(const char * s){pData=new Data(s);}
    AString(const char & c){pData=new Data(c);}
    AString( Data * p):pData(p){}
    ~AString();
    

    // Assignments
    AString		&operator=(const AString &);
    AString		&operator=(const char *);

    // Conversions
    inline		operator char *() ;
    inline		operator const char *() ;
    inline		operator const char *() const;

    // Operators
    friend inline ostream	&operator<<(ostream &, const AString &);
    friend istream		&operator>>(istream &, AString &);
    friend inline int		operator==(const AString &, const AString &);
    friend inline int		operator==(const AString &, const char *);
    friend inline int		operator==(const char *, const AString &);
    friend inline int		operator!=(const AString &, const AString &);
    friend inline int		operator!=(const AString &, const char *);
    friend inline int		operator!=(const char *, const AString &);
    friend inline int		operator<(const AString &, const AString &);
    friend inline int		operator<(const AString &, const char *);
    friend inline int		operator<(const char *, const AString &);
    friend inline int		operator>(const AString &, const AString &);
    friend inline int		operator>(const AString &, const char *);
    friend inline int		operator>(const char *, const AString &);
    friend inline int		operator<=(const AString &, const AString &);
    friend inline int		operator<=(const AString &, const char *);
    friend inline int		operator<=(const char *, const AString &);
    friend inline int		operator>=(const AString &, const AString &);
    friend inline int		operator>=(const AString &, const char *);
    friend inline int		operator>=(const char *, const AString &);

    AString 		operator+(const AString &);
    AString 		operator+(const char *);
    friend AString 		operator+(const char *, const AString &);
    AString			&operator+=(const AString &);
    AString			&operator+=(const char *);
    AString			operator()(int , int );
    AString			operator()(int  , int ) const;
    AString			operator()(int  );
    AString			operator()(int   ) const;
    inline char			operator[](int) const;
    inline char			&operator[](int);

    // Additional member functions
    inline unsigned int	length() const;
    AString		upper() const;
    AString		lower() const;
    int			index(const AString &) const;


};

inline AString::operator char *(
    ) 

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is a conversion operator from AString to char *.
//
//  RETURN VALUE:
//
//      The char * representation of the AString, as internally stored in the
//	pString part of its pDataprivate data member.
//--

{
   _mutator(); return pData->pString;
}
inline AString::operator const char *(
    ) 

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is a conversion operator from AString to const char *.
//
//  RETURN VALUE:
//
//      The char * representation of the AString, as internally stored in the
//	pString part of its pDataprivate data member.
//--

{
    return (const char *)(pData->pString);
}
inline AString::operator const char *(
    ) const

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is a conversion operator from AString to const char *.
//
//  RETURN VALUE:
//
//      The char * representation of the AString, as internally stored in the
//	pString part of its pDataprivate data member.
//--

{
    return (const char *)(pData->pString);
}



inline ostream &operator<<(
    ostream	 &os,  // Reference to the ostream object.
    const AString &str  // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the output operator, inserting the contents of a AString object,
//	specified by the str argument, into an ostream object, specified by the
//	os argument.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      Reference to the ostream object used for output.
//--

{
    return os << str.pData->pString;
}



inline int operator==(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the equality operator used to compare character strings
//	associated with two AString objects, specified by the str_1 and str_2
//	arguments.  The terms of comparison are as defined for the ANSI C
//	strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are equal; 0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) == 0;
}



inline int operator==(
    const AString &str,  // Reference to the AString object.
    const char	 *ptr   // Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the equality operator used to compare the character string
//	associated with a AString object, specified by the str argument, with a
//	character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are equal; 0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) == 0;
}



inline int operator==(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the equality operator used to compare a character string,
//	specified by the ptr argument, with the character string associated
//	with a AString object, specified by the str argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are equal; 0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) == 0;
}



inline int operator!=(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the inequality operator used to compare character strings
//	associated with two AString objects, specified by the arguments str_1
//	and str_2, respectively.  The terms of comparison are as defined for
//	the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are not equal; 0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) != 0;
}



inline int operator!=(
    const AString &str,  // Reference to the AString object.
    const char   *ptr   // Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the inequality operator used to compare the character string
//	associated with a AString object, specified by the str argument, with a
//	character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are not equal; 0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) != 0;
}



inline int operator!=(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the inequality operator used to compare a character string,
//	specified by the ptr argument, with the character string associated
//	with a AString object, specified by the str argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//      1 if the respective character strings are not equal; 0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) != 0;
}



inline int operator<(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the less-than operator used to compare character strings
//	associated with two AString objects, specified by the arguments str_1
//	and str_2, respectively.  The terms of comparison are as defined for
//	the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is less than the second; 0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) < 0;
}



inline int operator<(
    const AString &str,	// Reference to the AString object.
    const char   *ptr	// Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the less-than operator used to compare the character string
//	associated with a AString object, specified by the str argument, with a
//	character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is less than the second; 0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) < 0;
}



inline int operator<(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the less-than operator used to compare a character string,
//	specified by the ptr argument, with the character string associated
//	with a AString object, specified by the str argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is less than the second; 0 otherwise.
//--

{
    return strcmp(ptr, str.pData->pString) < 0;
}



inline int operator>(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the greater-than operator used to compare character strings
//	associated with two AString objects, specified by the str_1 and str_2
//	arguments, respectively.  The terms of comparison are as defined for
//	the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is greater than the second;
//	0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) > 0;
}


inline int operator>(
    const AString &str,  // Reference to the AString object.
    const char   *ptr   // Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the greater-than operator used to compare the character string
//	associated with a AString object, specified by the str argument, with a
//	character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is greater than the second;
//	0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) > 0;
}


inline int operator>(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the greater-than operator used to compare a character string,
//	specified by the ptr argument, with the character string associated
//	with a AString object, specified by the str argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is greater than the second;
//	0 otherwise.
//--

{
    return strcmp(ptr, str.pData->pString) > 0;
}


inline int operator<=(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-greater-than operator used to compare character strings
//	associated with two AString objects, specified by the str_1 and str_2
//	arguments, respectively.  The terms of comparison are as defined for
//	the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not greater than the second;
//	0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) <= 0;
}



inline int operator<=(
    const AString &str,  // Reference to the AString object.
    const char   *ptr   // Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-greater-than operator used to compare the character
//	string associated with a AString object, specified by the str argument,
//	with a character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not greater than the second;
//	0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) <= 0;
}



inline int operator<=(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-greater-than operator used to compare a character
//	string, specified by the ptr argument, with the character string
//	associated with a AString object, specified by the str argument.  The
//	terms of comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not greater than the second;
//	0 otherwise.
//--

{
    return strcmp(ptr, str.pData->pString) <= 0;
}



inline int operator>=(
    const AString &str_1,  // Reference to the first AString object.
    const AString &str_2   // Reference to the second AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-less-than operator used to compare character strings
//	associated with two AString objects, specified by the str_1 and str_2
//	arguments, respectively.  The terms of comparison are as defined for
//	the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not less than the second;
//	0 otherwise.
//--

{
    return strcmp(str_1.pData->pString, str_2.pData->pString) >= 0;
}



inline int operator>=(
    const AString &str,  // Reference to the AString object.
    const char   *ptr   // Pointer to the character string.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-less-than operator used to compare the character string
//	associated with a AString object, specified by the str argument, with a
//	character string, specified by the ptr argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not less than the second;
//	0 otherwise.
//--

{
    return strcmp(str.pData->pString, ptr) >= 0;
}



inline int operator>=(
    const char   *ptr,  // Pointer to the character string.
    const AString &str   // Reference to the AString object.
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is the not-less-than operator used to compare a character string,
//	specified by the ptr argument, with the character string associated
//	with a AString object, specified by the str argument.  The terms of
//	comparison are as defined for the ANSI C strcmp function.
//
//  FRIEND OF:	AString
//
//  RETURN VALUE:
//
//	1 if the first character string is not less than the second;
//	0 otherwise.
//--

{
    return strcmp(ptr, str.pData->pString) >= 0;
}



inline char AString::operator[](
    int position 	// Index into string
    )
    const

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is a subscript operator ([]) which returns a character within
//	the string.
//
//  RETURN VALUE:
//
//	The character at the requested position within the string.
//	If the index is pointing past the end of the string, 0 is
//	returned.  If the index is negative, the results are undefined.
//--

{
    return (position < length() && position>=0) ? pData->pString[position] : 0;
}



inline char & AString::operator[](
    int position 	// Index into string
    )

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This is a subscript operator ([]) which returns the reference to a 
//	character within the string.
//
//  RETURN VALUE:
//
//	The reference to the character at the requested position within the 
//	string.  This reference is potentially invalid after any subsequent 
//	call to a non-const member function for the object.
//	
//	If the index is pointing past the end of the string or if the index
//	is negative, the results are undefined.
//--
{
    _mutator();return pData->pString[position];
}


inline unsigned int AString::length(
    ) const

//++
//  FUNCTIONAL DESCRIPTION:
//
//	This member function returns the length of the character string
//	associated with the AString object.
//
//  RETURN VALUE:
//
//      The length of the character string, as defined for the ANSI C strlen
//	function.
//--

{
    return strlen(pData->pString)+1;
}

#endif
