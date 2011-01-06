Storage types in C++
---------------------
    *Constant Data
        Values are known at compile-time.
        This kind of data stored outside of the program read-write area.
        Therefore write attempt is undefined.
            const char *hello1 = "Hello world";
            char *hello2 = "Other hello";
            // hello1[1] = 'a';                     // syntax error
            hello2[1] = 'a';                        // could cause runtime error
            char *s = const_cast<char *>(hello1);   // dangerous
            s[3] = 'x';                             // could be runtime error !
            
        There is difference between a string literal and an initialized character array.
            int main()
            {
                // declaration of three arrays in the user data area
                // read and write permissions for the elements:
                char t1[] = {'H','e','l','l','o','\0'};
                char t2[] = "Hello";
                char t3[] = "Hello";
    
                // declaration of two pointers in the user data area
                // read and write permissions for the pointers
                // ...and...
                // allocation of the "Hello" literal (possibly) read-only
                char  *s1 = "Hello";    // s1 points to 'H'
                char  *s2 = "Hello";    // ... and s2 likely points to the same place  
    
                void  *v1 = t1, *v2 = t2, *v3 = t3, *v4 = s1, *v5 = s2;
                std::cout <<v1<<'\t'<<v2<<'\t'<<v3<<'\t'<<v4<<'\t'<<v5<<std::endl;
                // the result (v1, v2 v3 are different, v4 and v5 could be the same):
                0xbffff460   0xbffff450    0xbffff440   0x8048844   0x8048844
            
                // assignment to array elements:
                *t1 = 'x'; *t2 = 'q'; *t3 = 'y';
            
                // modifying string literal: could be segmentation error:
                *s1 = 'w'; *s2 = 'z';
            
                return 0;
            }
    
    *Automatic Life
        Objects local to a block (and not declared static) has automatic life.
        Such objects are created in the stack. The stack is safe in a multithreaded environment.
        Objects created when the declaration is encountered and destroyed when controll leaves the declaration block.
            void f()
            {
                int i = 2;  // life starts here with initialization
                ....
            }               // life finished here
    
    *Dynamic Life
        Objects with dynamic life is created in the free store. The lifetime starts with the evaluation of a
        new  expression (not the new operator). The life ends at the delete expression.
            char *p = new char[1024];   // life starts here
            ...
            delete [] p;                // life finished here
    
    *Static Life
        Global variables, namespace variables, and static datamembers have static life.
        Static life starts at the beginning of the program, and ends at the end of the program.
        The order of creation is well-defined inside a compilation unit, but no defined order between source-files
            date d(2003,3,13);      // life of "d", "i" starts here
            static int i;
            int main()
            {                       // initialization/constr call happens here
            ...
            }                       // destr call happens here
    
    *Local Static Variables
        Local statics are declared inside a function as local variables, but with the static keyword.
        The life starts (and the initialization happens) when the declaration first time encountered and ends when the program is finishing.
            int main()
            {
                while (... )
                    if ( ... ) {
                        static int j = 6;   // initialization happens here
                    }
            }                               // destr call happens here
            

Internal/External Declaration - Internal/External Linkage
---------------------------------------------------------
(see http://publications.gbdirect.co.uk/c_book/chapter4/linkage.html)
 
There are essentially two types of object in C: the internal and external objects.
The distinction between external and internal is to do with functions: anything declared outside
a function is external, anything inside one, including its formal parameters, is internal.
Since no function can be defined inside another, functions themselves are always external.
At the outermost level, a C program is a collection of external objects.

The term used by the Standard to describe the accessibility of objects from one file to another,
or even within the same file, is linkage. There are three types of linkage: external linkage,
internal linkage and no linkage. Anything internal to a function—its arguments, variables and so
on—always has no linkage and so can only be accessed from inside the function itself

Objects that have external linkage are all considered to be located at the outermost level of the program;
this is the default linkage for functions and anything declared outside of a function.
All instances of a particular name with external linkage refer to the same object in the program. 

Names with internal linkage only refer to the same object within a single source file.
You do this by prefixing their declarations with the keyword static, which changes the linkage 
of external objects from external linkage to internal linkage. It is also possible to declare 
internal objects to be static, but that has an entirely different meaning which we can defer for the moment.

It's confusing that the types of linkage and the types of object are both described by the terms ‘internal’
and ‘external’; this is to some extent historical. C archaeologists may know that at one time the two
were equivalent and one implied the other—for us it's unfortunate that the terms remain but the meanings 
have diverged.

To summarize:
    Type of linkage 	Type of object 	                    Accessibility
    ¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
    external            external                            throughout the program
    internal            external(use static keyword)        a single file
    none                internal                            local to a single function
    
Here are the combinations of the use of the keywords, the types of declarations and the resulting linkage:
	Declaration	    Keyword     Resulting Linkage 	Accessibility 	    Note
	¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯¯
	external        none        external            entire program      2
	external        extern      external            entire program      2
	external        static      internal            a single file       2
	internal        none        none                a single function 	
	internal        extern      external            entire program      1
	internal        static      none                a single function 	2
	
1) Although the accessibility of internal declarations prefixed with extern is program-wide, watch out
   for the scope of the name.
2) External (or internal static) objects are initialized once only, at program start-up.
   The absence of explicit initialization is taken to be a default initialization of zero.

Memory Map for a C/C++ Program
-------------------------------
    Stack 	        Local variables, Return values, Return addresses
    Heap 	        Free memory for dynamic allocation
    Data Region 	External variables, Static variables, String constants
    Text Region 	Program instructions    


String Pointers Nomenclature
----------------------------

LPCTSTR = Long Pointer to a Const TCHAR STRing  (Long Pointer is the same as a pointer. There were two flavors of pointers under 16-bit windows.)
LPSTR = char*
LPCSTR = const char*
LPWSTR = wchar_t*
LPCWSTR = const wchar_t*
LPTSTR = char* or wchar_t* depending on _UNICODE
LPCTSTR = const char* or const wchar_t* depending on _UNICODE


C++ Operators priority
----------------------

Priority	Operator		Description				Associativity
1		::			scope					Left
2		() [ ] - . sizeof	 					Left
3 		++ --			increment/decrement			Right
3		~			Complement to one (bitwise)		Right
3		!			unary NOT				Right
3		& *			Reference and Dereference (pointers)	Right
3		(type)			Type casting				Right
3		+ -			Unary less sign				Right
4		* / %			arithmetical operations			Left
5		+ -			arithmetical operations			Left
6		<< 			bit shifting (bitwise)			Left
7		< <= =			Relational operators			Left
8		== !=			Relational operators			Left
9		& ^ |			Bitwise operators			Left
10		&& ||			Logic operators				Left
11		?:			Conditional				Right
12		= += -= *= /= %=	Assignation				Right
12		= <<= &= ^= |=		Assignation				Right
13		,			Comma, Separator			Left


