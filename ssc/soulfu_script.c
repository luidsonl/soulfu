#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "soulfu_script.h"

#ifndef FALSE
#    define FALSE 0
#endif

#ifndef TRUE
#    define TRUE 1
#endif

#define log_message
#define debug	printf

#define repeat(A, B) for(A=0;  A<B;  A++)

#define SRC_PERMANENT				(0)	// Define is permanent
#define SRC_TEMPORARY_FILE			(1)	// Define goes away when done with file
#define SRC_TEMPORARY_FUNCTION		(2)	// Define goes away when done with function

#define SRC_MAX_TOKEN              64   // The maximum number of pieces per line of a SRC file
#define SRC_MAX_DEFINE           2048   // The maximum number of defines
#define SRC_MAX_TOKEN_SIZE        128   // The maximum size of each piece

#define SRC_MAX_INDENT            256   // Allow up to 256 indentation levels...
#define SRC_JUMP_INVALID            0   // Marked as unused...
#define SRC_JUMP_IF                 1   // Used when an if is found
#define SRC_JUMP_ELSE               2   // Used when an else is found
#define SRC_JUMP_WHILE              3   // Used when a while is found

// For direct code->script calls...
#  define MAX_FAST_FUNCTION          16  // For predefined script functions like Spawn
#  define FAST_FUNCTION_SPAWN        0   // Offset for the Spawn() function
#  define FAST_FUNCTION_REFRESH      2   // Offset for the Refresh() function
#  define FAST_FUNCTION_EVENT        4   // Offset for the Event() function
#  define FAST_FUNCTION_AISCRIPT     6   // Offset for the AIScript() function
#  define FAST_FUNCTION_BUTTONEVENT  8   // Offset for the ButtonEvent() function
#  define FAST_FUNCTION_GETNAME      10  // Offset for the GetName() function
#  define FAST_FUNCTION_UNPRESSED    12  // Offset for the Unpressed() function
#  define FAST_FUNCTION_FRAMEEVENT   14  // Offset for the FrameEvent() function
#  define FAST_FUNCTION_MODELSETUP   16  // Offset for the ModelSetup() function...
#  define FAST_FUNCTION_DEFENSERATING 18 // Offset for the DefenseRating() function...
#  define FAST_FUNCTION_SETUP        20  // Offset for the Setup() function...
#  define FAST_FUNCTION_DIRECTUSAGE  22  // Offset for the DirectUsage() function...
#  define FAST_FUNCTION_ENCHANTUSAGE 24  // Offset for the EnchantUsage() function...

#define MAX_VARIABLE               32   // I00 - I31, F00 - F31...
#define MAX_ARGUMENT               16

#define OPCODE_EQUALS                   0
#define OPCODE_ADD                      1
#define OPCODE_SUBTRACT                 2
#define OPCODE_MULTIPLY                 3
#define OPCODE_DIVIDE                   4
#define OPCODE_INCREMENT                5
#define OPCODE_DECREMENT                6
#define OPCODE_ISEQUAL                  7
#define OPCODE_ISNOTEQUAL               8
#define OPCODE_ISGREATEREQUAL           9
#define OPCODE_ISLESSEREQUAL           10
#define OPCODE_ISGREATER               11
#define OPCODE_ISLESSER                12
#define OPCODE_LOGICALAND              13
#define OPCODE_LOGICALOR               14
#define OPCODE_LOGICALNOT              15
#define OPCODE_NEGATE                  16
// !!!BAD!!!
// !!!BAD!!!
#define OPCODE_BITWISEAND              19
#define OPCODE_BITWISEOR               20
#define OPCODE_BITWISELEFT             21
#define OPCODE_BITWISERIGHT            22
#define OPCODE_MODULUS                 23
#define OPCODE_LOCALMESSAGE            24
#define OPCODE_LOGMESSAGE              25
#define OPCODE_FINDSELF                26
#define OPCODE_SYSTEMSET               27
#define OPCODE_SYSTEMGET               28
#define OPCODE_DEBUGMESSAGE            29
#define OPCODE_TOFLOAT                 30
#define OPCODE_TOINT                   31
#define OPCODE_F_EQUALS                32
#define OPCODE_F_ADD                   33
#define OPCODE_F_SUBTRACT              34
#define OPCODE_F_MULTIPLY              35
#define OPCODE_F_DIVIDE                36
#define OPCODE_F_INCREMENT             37
#define OPCODE_F_DECREMENT             38
#define OPCODE_F_ISEQUAL               39
#define OPCODE_F_ISNOTEQUAL            40
#define OPCODE_F_ISGREATEREQUAL        41
#define OPCODE_F_ISLESSEREQUAL         42
#define OPCODE_F_ISGREATER             43
#define OPCODE_F_ISLESSER              44
#define OPCODE_F_LOGICALAND            45
#define OPCODE_F_LOGICALOR             46
#define OPCODE_F_LOGICALNOT            47
#define OPCODE_F_NEGATE                48
#define OPCODE_STRING                  49
#define OPCODE_STRINGGETNUMBER         50
#define OPCODE_STRINGCLEAR             51
#define OPCODE_STRINGCLEARALL          52
#define OPCODE_STRINGAPPEND            53
#define OPCODE_STRINGCOMPARE           54
#define OPCODE_STRINGLENGTH            55
#define OPCODE_STRINGCHOPLEFT          56
#define OPCODE_STRINGCHOPRIGHT         57
#define OPCODE_STRINGRANDOMNAME        58
#define OPCODE_STRINGSANITIZE          59
#define OPCODE_NETWORKMESSAGE          60
#define OPCODE_STRINGLANGUAGE          61
#define OPCODE_STRINGUPPERCASE         62
#define OPCODE_STRINGAPPENDNUMBER      63
#define OPCODE_CALLFUNCTION            64
#define OPCODE_RETURNINT               65
#define OPCODE_RETURNFLOAT             66
#define OPCODE_IFFALSEJUMP             67
#define OPCODE_JUMP                    68
#define OPCODE_SQRT                    69
#define OPCODE_FILEOPEN                70
#define OPCODE_FILEREADBYTE            71
#define OPCODE_FILEWRITEBYTE           72
#define OPCODE_FILEINSERT              73
#define OPCODE_SPAWN                   74
#define OPCODE_GOPOOF                  75
#define OPCODE_DISMOUNT                76
#define OPCODE_ROLLDICE                77
#define OPCODE_PLAYSOUND               78
#define OPCODE_PLAYMEGASOUND           79
#define OPCODE_DISTANCESOUND           80
#define OPCODE_PLAYMUSIC               81
#define OPCODE_UPDATEFILES             82
#define OPCODE_SIN                     83
#define OPCODE_ACQUIRETARGET           84
#define OPCODE_FINDPATH                85
#define OPCODE_BUTTONPRESS             86
#define OPCODE_AUTOAIM                 87
#define OPCODE_ROOMHEIGHTXY            88
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
#define OPCODE_WINDOWBORDER            96
#define OPCODE_WINDOWSTRING            97
#define OPCODE_WINDOWMINILIST          98
#define OPCODE_WINDOWSLIDER            99
#define OPCODE_WINDOWIMAGE            100
#define OPCODE_WINDOWTRACKER          101
#define OPCODE_WINDOWBOOK             102
#define OPCODE_WINDOWINPUT            103
#define OPCODE_WINDOWEMACS            104
#define OPCODE_WINDOWMEGAIMAGE        105
#define OPCODE_WINDOW3DSTART          106
#define OPCODE_WINDOW3DEND            107
#define OPCODE_WINDOW3DPOSITION       108
#define OPCODE_WINDOW3DMODEL          109
#define OPCODE_MODELASSIGN            110
#define OPCODE_PARTICLEBOUNCE         111
#define OPCODE_WINDOWEDITKANJI        112
#define OPCODE_WINDOW3DROOM           113
#define OPCODE_INDEXISLOCALPLAYER     114
#define OPCODE_FINDBINDING            115
#define OPCODE_FINDTARGET             116
#define OPCODE_FINDOWNER              117
#define OPCODE_FINDINDEX              118
#define OPCODE_FINDBYINDEX            119
#define OPCODE_FINDWINDOW             120
#define OPCODE_FINDPARTICLE           121
// !!!BAD!!!
#define OPCODE_ATTACHTOTARGET         123
#define OPCODE_GETDIRECTION           124
#define OPCODE_DAMAGETARGET           125
#define OPCODE_EXPERIENCEFUNCTION     126
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!
// Last basic function is 127...

#define VAR_INVALID               '?'   // Used a bunch
#define VAR_INT                   'I'   // Used a bunch
#define VAR_FLOAT                 'F'   // Used a bunch
#define VAR_STRING                'S'   // For property extensions only
#define VAR_TEXT                  'T'   // For property extensions only
#define VAR_BYTE                  'B'   // For property extensions only
#define VAR_WORD                  'W'   // For property extensions only

#define SRC_REQUEST_ENTRY       0   // Used for mega_find...  Return pointer to function header
#define SRC_REQUEST_OFFSET      1   // Tells us where a CallFunction should take us
#define SRC_REQUEST_ARGUMENTS   2   // Return pointer to a function's argument/returncode string
#define SRC_REQUEST_FILESTART   3   // Return pointer to the start of the function's run file data

static char *working_dir = NULL;
struct Buffer tempbuff;

static enum SSError error = SSE_NONE;

static char define_token[SRC_MAX_DEFINE][SRC_MAX_TOKEN_SIZE];  // ex. "TRUE"
static char define_value[SRC_MAX_DEFINE][SRC_MAX_TOKEN_SIZE];  // ex. "1"
static char define_temporary_level[SRC_MAX_DEFINE];            // 0 is global, 1 is file, 2 is function

static int src_num_define = 0;         // The number of defined values...  May be higher than actual...

static char *sdf_read_file = NULL;     // A pointer to the current read position for sdf_open
static int sdf_read_first_line;        // FALSE until sdf_read_line has been called...
static int sdf_read_line_number = 0;   // The current line number
static int sdf_read_remaining;         // The number of bytes left to read

static unsigned char token_buffer[SRC_MAX_TOKEN][SRC_MAX_TOKEN_SIZE];   // A place to put the pieces
static signed char next_token_may_be_negative;     // For reading -5 as negative 5 instead of minus 5

// for compilerizing only
static unsigned char last_function_returns_integer;
unsigned char last_return_type;
static signed char float_variable_set[MAX_VARIABLE];  // Make sure variables are set before they're
static signed char int_variable_set[MAX_VARIABLE];    // used in expressions... x = 0 before y = x...
#define SRC_BUFFER_SIZE 65550  //16384 extra big so src_buffer_used doesn't write other stuff... Size of the RUN (!!!) file...
static unsigned char src_buffer[SRC_BUFFER_SIZE];  // Stick the RUN file here while building it...
static unsigned short src_buffer_used = 0;         // Current size
unsigned char token_order;                  // For determining RPN order/priority...
// Property stuff...
#define MAX_PROPERTY 256  // Must be 256...  Or else script props need to have 2 byte extensions...
char property_token[MAX_PROPERTY][8];         // Tags for the properties...  the x of window.x...
char property_type[MAX_PROPERTY];             // F or I or others...
unsigned short property_offset[MAX_PROPERTY]; // Offset of data for this property
int obj_num_property;                         // The number of registered properties

unsigned short put_jump_offset_here[SRC_MAX_INDENT];     // For figurin' out indentation jumps
unsigned short while_jumps_back_to_here[SRC_MAX_INDENT]; // For figurin' out indentation jumps
unsigned char last_jump_type_found[SRC_MAX_INDENT];     // Not used, If, Else, While...
unsigned char token_priority[SRC_MAX_TOKEN];                     // For figurin' out the RPN order
unsigned char token_priority_list[SRC_MAX_TOKEN];                // List of tokens in order...
unsigned char token_is_operand[SRC_MAX_TOKEN];                   // For figurin' out the RPN order
unsigned char token_is_operator[SRC_MAX_TOKEN];                  // For figurin' out the RPN order
unsigned char token_is_function[SRC_MAX_TOKEN];                  // For figurin' out the RPN order
unsigned char token_opcode[SRC_MAX_TOKEN];                       // For figurin' out the RPN data
unsigned char token_variable_type[SRC_MAX_TOKEN];                // F or I or ?...
signed char token_number_to_destroy[SRC_MAX_TOKEN];              // The number of arguments to a function
unsigned char token_change_type[SRC_MAX_TOKEN];                  // For variable type conversions
unsigned char token_is_destroyed[SRC_MAX_TOKEN];                 // For variable type conversions
unsigned char token_is_string[SRC_MAX_TOKEN];                    // 'Cause string pointers are filled in during functionize...
unsigned char* token_arg_list[SRC_MAX_TOKEN];                    // Points to somethin' like "FII" for a function...
unsigned char line_is_a_conditional;                             // While, If, or Else on line...
int token_extension[SRC_MAX_TOKEN];                     // For figurin' out the RPN data

char arg_list_none[]="";                    // Argument lists for basic functions
char arg_list_int[]="I";                    //
char arg_list_int_int[]="II";               //
char arg_list_float[]="F";                  //
char arg_list_float_float[]="FF";           //
char arg_list_iii[] = "III";                //
char arg_list_iif[] = "IIF";                //
char arg_list_iiii[] = "IIII";              //
char arg_list_iffi[] = "IFFI";              //
char arg_list_ffii[] = "FFII";              //
char arg_list_iiiii[] = "IIIII";            //
char arg_list_ifffi[] = "IFFFI";            //
char arg_list_ffiii[] = "FFIII";            //
char arg_list_iiiiii[] = "IIIIII";          //
char arg_list_iffiii[] = "IFFIII";          //
char arg_list_ffiiii[] = "FFIIII";          //
char arg_list_ffiiiii[] = "FFIIIII";        //
char arg_list_fffi[] = "FFFI";              //
char arg_list_fffiiii[] = "FFFIIII";        //
char arg_list_ffffi[] = "FFFFI";            //
char arg_list_ffffiii[] = "FFFFIII";        //
char arg_list_ffffiiiiii[] = "FFFFIIIIII";  //
char arg_list_ffffffffffffiii[] = "FFFFFFFFFFFFIII";
// ---

static void src_undefine_level(char temporary_level);
static float sdf_read_float(unsigned char* location);
static unsigned int sdf_read_unsigned_int(unsigned char* location);
static void sdf_write_unsigned_int(unsigned char* location, unsigned int value);
static unsigned short sdf_read_unsigned_short(unsigned char* location);
static void sdf_write_unsigned_short(unsigned char* location, unsigned short value);
static signed char sdf_read_line(void);
static signed char src_read_token(unsigned char* buffer);
static int count_indentation(char *string);
static void src_add_define(char* token, char* value, char temporary_level);
static int src_get_define(char* token);
static void make_uppercase(char *string);

// compilerize only
int obj_get_property(char* token);
void src_close_jumps(unsigned char indent, unsigned char last_indent);
void src_add_return_opcode(void);
int src_find_function_entry(unsigned char* filedata, char* functionname);
void src_make_arrays(int token_count, char* filename);
void src_find_priority(int start, int end, signed char change_signs, signed char first_check);
void src_figure_variable_types(int token_count);
void src_generate_opcodes(int token_count);
int src_find_string_entry(unsigned char* filedata, char* stringname);
unsigned char* src_mega_find_function(unsigned char* functionstring, unsigned char* filename, unsigned char request);
void src_set_priority(int start, int i, int end, signed char any_type);
unsigned char* sdf_find_filetype(char *filename, char *filetype);
void obj_reset_property(void);
void obj_add_property(char* tag, char type, char* offset);
signed char sdf_open(char* filename);
// ---

enum SSError src_headerize(struct Buffer *script, struct Buffer *run)
{
	// This function runs through an SRC file that has been stored in memory and finds
	// function information to store in the header.  This must be done for every file
	// before compilation can occur.

	error = SSE_NONE;

	// Undefine any variables and defines used by the last file
	src_undefine_level(SRC_TEMPORARY_FILE);

	// Just like sdf_open...
	sdf_read_first_line = FALSE;
	sdf_read_remaining = script->used;
	sdf_read_file = (unsigned char *)script->mem;
	sdf_read_line_number = 0;

	next_token_may_be_negative = TRUE;

	// Pad the start of the header with fast function offsets...
	run->used = 0;
	for (int i = 0; i < MAX_FAST_FUNCTION; ++i)
	{
		// 2 bytes each...
		run->mem[run->used] = 0;
		run->used++;
		run->mem[run->used] = 0;
		run->used++;
	}
	// Something like this in the run_buffer...
	//   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00


	// Make a list of all the functions in this file...  Figure out offsets in another pass...
	run->used += 4;  // Skip a word for the string count...  Filled in after counting...
	//debug("Headerizing functions...\n");
	unsigned short number_of_functions = 0;
	while (sdf_read_line())
	{
		// Count the indentation (by 2's), and skip over any whitespace...
		unsigned char indent = count_indentation(sdf_read_file);
		sdf_read_file += indent;
		sdf_read_remaining -= indent;
		indent = indent >> 1;

		// Skip #defines, we're only interested in functions...
		if (sdf_read_file[0] != '#' && indent == 0)
		{
			// Read all of the tokens on this line...
			int token_count = 0;
			unsigned short keepgoing = TRUE;
			while (keepgoing)
			{
				keepgoing = FALSE;
				if (token_count < SRC_MAX_TOKEN)
				{
					if (src_read_token(token_buffer[token_count]))
					{
						//debug("Line %d, %s\n", sdf_read_line_number, token_buffer[token_count]);
						token_count++;
						keepgoing = TRUE;
					}
				}
			}

			// Did we read anything?
			if (token_count > 0)
			{
				// Get the return type for the function...
				int i = 0;
				char returncode = 'I';
				if (strcmp(token_buffer[0], "INT") == 0)  { returncode = 'I';  i++; }
				if (strcmp(token_buffer[0], "FLOAT") == 0)  { returncode = 'F';  i++; }

				// Make sure the function has parentheses after it
				if (token_buffer[i+1][0] == '(')
				{
					// Now stick the function address and name in our RUN file...
					sprintf((run->mem + run->used), "%c%c%s%c", 0, 0, token_buffer[i], 0);
					run->used += strlen(token_buffer[i]) + 3;
					//debug("function...  %s()\n", token_buffer[i]);

					// Now store the return code type...
					sprintf((run->mem + run->used), "%c", returncode);
					run->used++;

					// Now store any arguments types...
					i = i + 2;
					while (i < token_count)
					{
						if (strcmp(token_buffer[i], "INT") == 0)
						{
							sprintf((run->mem + run->used), "I");
							run->used++;
						}
						else if (strcmp(token_buffer[i], "FLOAT") == 0)
						{
							sprintf((run->mem + run->used), "F");
							run->used++;
						}
						i++;
					}

					// And zero terminate the string...
					sprintf((run->mem + run->used), "%c", 0);
					run->used++;
					number_of_functions++;
				}
				else
				{
					debug("Error: (line %d), function symbol '%s' not followed by ()\n", sdf_read_line_number, token_buffer[i]);
					error = SSE_HEADERIZE;
					goto src_headerize_error_handle;
				}
			}
		}
	}
	sdf_write_unsigned_short(run->mem + (MAX_FAST_FUNCTION << 1), number_of_functions);
	// Something like this in the run_buffer...
	//   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	//   00 03 00 00
	//   0  0  D  E  A  T  H  00 I  F  F  F  I  00
	//   0  0  B  L  A  H  00 V  00
	//   0  0  A  T  T  A  C  K  00 V  I  00
	//   Address     FunctionName      ReturnAndArgumentTypes

	// Check for errors too...
	if (number_of_functions == 0)
	{
		debug("Error: No functions found.\n");
		error = SSE_HEADERIZE;
		goto src_headerize_error_handle;
	}

	// Now search for any #defines...  Just in case someone #define'd a string...
	//debug("Headerizing #define's...\n");

	sdf_read_first_line = FALSE;
	sdf_read_remaining = script->used;
	sdf_read_file = (unsigned char *)script->mem;
	sdf_read_line_number = 0;

	next_token_may_be_negative = TRUE;

	// Make a list of all the functions in this file...  Figure out offsets in another pass...
	while (sdf_read_line())
	{
		// Count the indentation (by 2's), and skip over any whitespace...
		unsigned char indent = count_indentation(sdf_read_file);
		sdf_read_file += indent;
		sdf_read_remaining -= indent;
		indent = indent >> 1;

		// Check for a #define...
		if (sdf_read_file[0] == '#')
		{
			if (sdf_read_file[1] == 'D')
			{
				if (sdf_read_file[2] == 'E')
				{
					if (sdf_read_file[3] == 'F')
					{
						if (sdf_read_file[4] == 'I')
						{
							if (sdf_read_file[5] == 'N')
							{
								if (sdf_read_file[6] == 'E')
								{
									sdf_read_file+=7;
									sdf_read_remaining-=7;
									if (src_read_token(token_buffer[0]))
									{
										src_add_define(token_buffer[0], sdf_read_file, SRC_TEMPORARY_FILE);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Now search for strings to append to the header...  Just like above...
	sdf_read_first_line = FALSE;
	sdf_read_remaining = script->used;
	sdf_read_file = (unsigned char *)script->mem;
	sdf_read_line_number = 0;

	next_token_may_be_negative = TRUE;

	// Make a list of all the functions in this file...  Figure out offsets in another pass...
	//debug("Headerizing strings...\n");
	unsigned short number_of_strings = 0;
	while (sdf_read_line())
	{
		// Count the indentation (by 2's), and skip over any whitespace...
		unsigned char indent = count_indentation(sdf_read_file);
		sdf_read_file += indent;
		sdf_read_remaining -= indent;
		indent = indent >> 1;

		// Skip #defines, we're only interested in strings...
		if (sdf_read_file[0] != '#' && indent != 0)
		{
			// Read all of the tokens on this line...
			int token_count = 0;
			unsigned short keepgoing = TRUE;
			while (keepgoing)
			{
				keepgoing = FALSE;
				if (token_count < SRC_MAX_TOKEN)
				{
					if (src_read_token(token_buffer[token_count]))
					{
						token_count++;
						keepgoing = TRUE;
					}
				}
			}

			// Look through all of the tokens for a string token...
			for (int i = 0; i < token_count; ++i)
			{
				// Look for string tokens...
				//debug("Token %s\n", token_buffer[i]);

				if(token_buffer[i][0] == '"')
				{
					// Found a string, so put it in the header
					sdf_write_unsigned_short(run->mem + run->used, 65535);
					run->used += 2;
					int length = strlen(token_buffer[i]+1)-1;
					for (int j = 0; j < length; ++j)
					{
						*(run->mem + run->used) = token_buffer[i][j+1];
						run->used++;
					}
					*(run->mem + run->used) = 0;
					run->used++;
					number_of_strings++;
				}
				else
				{
					// Check for #define'd strings...
					int def = src_get_define(token_buffer[i]);
					if (def >= 0)
					{
						//debug("Sneaky define... %s\n", define_token[def]);

						// It was a sneaky #define...  Search for " characters...
						char read_char = define_value[def][0];
						int k = 0;
						while (read_char != 0)
						{
							if (read_char == '"')
							{
								// Start of a string, so write it into the header...
								sdf_write_unsigned_short(run->mem + run->used, 65535);
								run->used += 2;
								k++;
								read_char = define_value[def][k];
								while(read_char != 0 && read_char != '"')
								{
									*(run->mem + run->used) = read_char;
									run->used++;
									k++;
									read_char = define_value[def][k];
								}
								*(run->mem + run->used) = 0;
								run->used++;
								number_of_strings++;
								if(read_char == 0) k--;
							}
							k++;
							read_char = define_value[def][k];
						}
					}
				}
			}
		}
	}
	sdf_write_unsigned_short(run->mem + (MAX_FAST_FUNCTION << 1) + 2, number_of_strings);
	// Something like this in the run_buffer...
	//   00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	//   00 03 00 02
	//   0  0  D  E  A  T  H  00 I  F  F  F  I  00
	//   0  0  B  L  A  H  00 V  00
	//   0  0  A  T  T  A  C  K  00 V  I  00
	//   0  0  B  i  g     B  o  b     i  s     c  o  o  l  00
	//   FF FF T  e  s  t     s  t  r  i  n  g  00
	//   FF FF F  I  L  E  :  T  E  S  T  .  T  X  T  00
	//   String address           String

src_headerize_error_handle:
	return error;
}

enum SSError src_compilerize(struct Buffer *script, struct Buffer *run, char *filename)
{
	// <ZZ> This function compiles an SRC file that has been stored in memory.
	int i;
	unsigned char indent;
	unsigned char last_indent;
	unsigned char* data;    // Original, Headerized data
	signed char keepgoing;
	int token_count;
	int num_int_variables = 0;
	int num_float_variables = 0;
	int define;
	int tempint;
	int tempnum;
	signed char tempnext;
	unsigned char* tempptr;
	signed char stillgoing;
	char operation_line;
	unsigned short number_of_functions;
	int function_offset;
	char tempstring[16];

	// Don't create the RUN file if we find any errors...
	error = SSE_NONE;

	// Log what we're doing
	//debug("Compilerizing...\n");

	// Undefine any variables and defines used by the last file
	src_undefine_level(SRC_TEMPORARY_FILE);

	// Clear out our jump data...
	repeat(i, SRC_MAX_INDENT)
	{
		last_jump_type_found[i] = SRC_JUMP_INVALID;
	}
	last_indent = 0;
	last_function_returns_integer = TRUE;

	// Remember what variables have been set (for generating errors)
	repeat(i, MAX_VARIABLE)
	{
		float_variable_set[i] = FALSE;
		int_variable_set[i] = FALSE;
	}

	// Copy all of the data from the header (that we already compiled)...
	src_buffer_used = run->used;
	memcpy(src_buffer, run->mem, src_buffer_used);

	// Just like sdf_open...
	sdf_read_first_line = FALSE;
	sdf_read_remaining = script->used;
	sdf_read_file = (unsigned char *)script->mem;
	sdf_read_line_number = 0;

	next_token_may_be_negative = TRUE;

	// Make a list of all the functions in this file...  Figure out offsets in another pass...
	while (sdf_read_line())
	{
		// Count the indentation (by 2's), and skip over any whitespace...
		indent = count_indentation(sdf_read_file);
		sdf_read_file += indent;
		sdf_read_remaining -= indent;
		indent = indent >> 1;

		// Check for a #define...
		if (sdf_read_file[0] == '#')
		{
			if (sdf_read_file[1] == 'd')
			{
				if (sdf_read_file[2] == 'e')
				{
					if (sdf_read_file[3] == 'f')
					{
						if (sdf_read_file[4] == 'i')
						{
							if (sdf_read_file[5] == 'n')
							{
								if (sdf_read_file[6] == 'e')
								{
									sdf_read_file += 7;
									sdf_read_remaining -= 7;
									if (src_read_token(token_buffer[0]))
									{
										src_add_define(token_buffer[0], sdf_read_file, SRC_TEMPORARY_FILE);
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Just like sdf_open...
	sdf_read_first_line = FALSE;
	sdf_read_remaining = script->used;
	sdf_read_file = (unsigned char *)script->mem;
	sdf_read_line_number = 0;

	// For each line in the file...
	number_of_functions = 0;
	while (sdf_read_line())
	{
		// Count the indentation (by 2's), and skip over any whitespace...
		indent = count_indentation(sdf_read_file);
		sdf_read_file += indent;
		sdf_read_remaining -= indent;
		indent = indent >> 1;

		next_token_may_be_negative = TRUE;

		if (sdf_read_file[0] != '#')
		{
			// Get all of the tokens in the line...
			token_count = 0;
			keepgoing = TRUE;
			while (keepgoing)
			{
				keepgoing = FALSE;
				if (token_count < SRC_MAX_TOKEN)
				{
					if (src_read_token(token_buffer[token_count]))
					{
						// Undefine any variables used by the last function
						if (indent == 0 && token_count == 0)
						{
							src_undefine_level(SRC_TEMPORARY_FUNCTION);
							num_int_variables = 0;
							num_float_variables = 0;
							// Remember what variables have been set (for generating errors)
							repeat(i, MAX_VARIABLE)
							{
								float_variable_set[i] = FALSE;
								int_variable_set[i] = FALSE;
							}
						}

						// See if the token is defined
						define = src_get_define(token_buffer[token_count]);
						if (define > -1)
						{
							// Save the read line stuff...
							tempint = sdf_read_remaining;
							tempptr = sdf_read_file;
							tempnum = sdf_read_line_number;
							tempnext = next_token_may_be_negative;

							// Read in all of the tokens in the #define...
							next_token_may_be_negative = TRUE;
							sdf_read_remaining = SRC_MAX_TOKEN_SIZE;
							sdf_read_file = define_value[define];
							stillgoing = TRUE;
							while (stillgoing)
							{
								stillgoing = FALSE;
								if (token_count < SRC_MAX_TOKEN)
								{
									if (src_read_token(token_buffer[token_count]))
									{
										token_count++;
										stillgoing = TRUE;
									}
								}
							}
							token_count--;

							// Restore the read line stuff...
							next_token_may_be_negative = tempnext;
							sdf_read_line_number = tempnum;
							sdf_read_file = tempptr;
							sdf_read_remaining = tempint;
						}

						// See if the token has a dot style property
						tempptr = strpbrk(token_buffer[token_count], ".");
						if (((token_buffer[token_count][0] >= 'A' && token_buffer[token_count][0] <= 'Z') || token_buffer[token_count][0] == '_') && tempptr)
						{
							// Looks like it is...  Need to break it into two parts...
							// First find the property extension...
							*tempptr = 0;
							tempint = obj_get_property(tempptr+1);
							define = src_get_define(token_buffer[token_count]);
							if(tempint > -1 && define > -1)
							{
								// Okay, we found the property and variable...  Now reguritate 'em
								// If we didn't find anything, it may be an external function...
								sprintf(token_buffer[token_count], "%s.%d", define_value[define], tempint);
							}
							else
							{
								// Must be an external function call...  Replace the period we removed...
								*tempptr = '.';
							}
						}

						token_count++;
						keepgoing = TRUE;
					}
				}
			}

			// Did we actually read anything on this line?
			if (token_count > 0)
			{
				// Check for errors with if and else and while with nothing after...
				if (line_is_a_conditional == TRUE && indent <= last_indent)
				{
					debug("Error: (line %d), Conditional without a nest\n", sdf_read_line_number);
					error = SSE_COMPILERIZE;
				}

				// Is this line the start of a function?
				operation_line = TRUE;
				if (indent == 0)
				{
					// It is...  Let's write down the offset so we can find it again later...
					operation_line = FALSE;

					if(last_indent > 0)
					{
						// Close out all jumps...
						src_close_jumps(indent, last_indent);

						// Slap in a return 1, just to be on the safe side...
						src_add_return_opcode();
						last_indent = 0;
					}

					// Skip over the return value...
					i = 0;
					last_function_returns_integer = TRUE;
					if (strcmp(token_buffer[0], "INT") == 0)  i++;
					if (strcmp(token_buffer[0], "FLOAT") == 0) { last_function_returns_integer = FALSE; i++; }

					// Find the function in the RUN header...
					function_offset = src_find_function_entry(src_buffer, token_buffer[i]);
					if (function_offset)
					{
						// Then write down the offset...
						sdf_write_unsigned_short(src_buffer + function_offset, src_buffer_used);
						number_of_functions++;
						i++;

						// Now define any arguments...  #define value i0
						i++;  // Skip the parentheses
						while (i < token_count)
						{
							if (strcmp(token_buffer[i], "INT") == 0 && num_int_variables < MAX_VARIABLE)
							{
								i++;
								if (i < token_count)
								{
									sprintf(tempstring, "I%d", num_int_variables);
									src_add_define(token_buffer[i], tempstring, SRC_TEMPORARY_FUNCTION);
									int_variable_set[num_int_variables] = TRUE;
									num_int_variables++;
								}
							}
							else if (strcmp(token_buffer[i], "FLOAT") == 0 && num_float_variables < MAX_VARIABLE)
							{
								i++;
								if (i < token_count)
								{
									sprintf(tempstring, "F%d", num_float_variables);
									src_add_define(token_buffer[i], tempstring, SRC_TEMPORARY_FUNCTION);
									float_variable_set[num_float_variables] = TRUE;
									num_float_variables++;
								}
							}
							i++;
						}
					}
					else
					{
						debug("Error: (line %d), Problem with function %s (corrupt header?)\n", sdf_read_line_number, token_buffer[i]);
						error = SSE_COMPILERIZE;
					}
				}

				// Check for local int and float declarations...  Use temporary defines... I0-I31, F0-F31
				if (indent == 1)
				{
					if (strcmp(token_buffer[0], "INT") == 0)
					{
						operation_line = FALSE;
						i = 1;
						while (i < token_count)
						{
							if (token_buffer[i][0] != ','  && num_int_variables < MAX_VARIABLE)
							{
								sprintf(tempstring, "I%d", num_int_variables);
								src_add_define(token_buffer[i], tempstring, SRC_TEMPORARY_FUNCTION);
								num_int_variables++;
							}
							i++;
						}
					}
					else if (strcmp(token_buffer[0], "FLOAT") == 0)
					{
						operation_line = FALSE;
						i = 1;
						while (i < token_count)
						{
							if (token_buffer[i][0] != ',' && num_float_variables < MAX_VARIABLE)
							{
								sprintf(tempstring, "F%d", num_float_variables);
								src_add_define(token_buffer[i], tempstring, SRC_TEMPORARY_FUNCTION);
								num_float_variables++;
							}
							i++;
						}
					}
				}

				if (operation_line)
				{
					// Must have a conditional to change indentation levels...
					if (indent > last_indent && last_indent != 0)
					{
						if (last_jump_type_found[last_indent] == SRC_JUMP_INVALID)
						{
							// Throw an error
							debug("Error: (line %d), Must have a conditional statement to indent\n", sdf_read_line_number);
							error = SSE_COMPILERIZE;
						}
					}

					// Going down an indentation level allows us to fill in jump locations...
					src_close_jumps(indent, last_indent);

					// Remember where while's start, because we need to jump back to 'em...
					if (strcmp(token_buffer[0], "WHILE") == 0)
					{
						while_jumps_back_to_here[indent] = src_buffer_used;
					}

//                    // Going down an indentation level allows us to fill in jump locations...
//                    src_close_jumps(indent, last_indent);
//  Moved this up a couple lines because two whiles in a row would break it...
//  Fixed that problem, but something else mighta broken in the process...

					// Remember indentation level for next time
					last_indent = indent;

					// Clear out the RPN data and fill in some of the helper arrays...
					src_make_arrays(token_count, filename);

					// Figure out the RPN order...
					token_order = 1;
					src_find_priority(0, token_count - 1, FALSE, TRUE);

					// Figure out type casting based on RPN data...
					src_figure_variable_types(token_order - 1);

					// Write out the opcode data...
					src_generate_opcodes(token_order - 1);

					// Figure out where to write jump locations and stuff for indentation levels
					if (error == SSE_NONE)
					{
						if (strcmp(token_buffer[0], "IF") == 0)
						{
							// Make sure there's an if opcode where it should be...
							if (src_buffer_used >= 5)
							{
								if (src_buffer[src_buffer_used - 3] == OPCODE_IFFALSEJUMP)
								{
									put_jump_offset_here[indent] = src_buffer_used - 2;
									last_jump_type_found[indent] = SRC_JUMP_IF;
								}
								else
								{
									debug("Error: (line %d), Praxis syllabication error\n", sdf_read_line_number);
									error = SSE_COMPILERIZE;
								}
							}
							else
							{
								error = SSE_COMPILERIZE;
							}
						}
						if (strcmp(token_buffer[0], "WHILE") == 0)
						{
							// Make sure there's an if opcode where it should be...
							if (src_buffer_used >= 5)
							{
								if (src_buffer[src_buffer_used - 3] == OPCODE_IFFALSEJUMP)
								{
									put_jump_offset_here[indent] = src_buffer_used-2;
									last_jump_type_found[indent] = SRC_JUMP_WHILE;
								}
								else
								{
									debug("Error: (line %d), Praxis syllabication error\n", sdf_read_line_number);
									error = SSE_COMPILERIZE;
								}
							}
							else
							{
								error = SSE_COMPILERIZE;
							}
						}
					}
				}
			}
		}
	}

	// Clean up any unresolved jumps...
	src_close_jumps(indent, last_indent);

	// Slap in a return 1, just to be on the safe side...
	src_add_return_opcode();

	if (SSE_NONE == error)
	{
			// Should overwrite headerized RUN file...
			// Copy the data out of the buffer...
			memcpy(run->mem, src_buffer, src_buffer_used);
			run->used = src_buffer_used;
	}
	else
	{
		debug("FOUND: Found error compilerizing\n");
	}

	return error;
}

static void src_undefine_level(char temporary_level)
{
	// <ZZ> This function gets rid of any defines that we don't need
	for (int i = 0; i < src_num_define; ++i)
	{
		if(define_temporary_level[i] >= temporary_level)
		{
			define_token[i][0] = 0;
			define_value[i][0] = 0;
			define_temporary_level[i] = 0;
		}
	}
}

static float sdf_read_float(unsigned char* location)
{
	// <ZZ> This function reads an float value at the given memory location, assuming that
	//      the memory is stored in big endian format.  The function returns the value it
	//      read.
	unsigned int value;
// !!!BAD!!!
// !!!BAD!!!  This might not work...
// !!!BAD!!!
	value  = *location;     value = value<<8;
	value |= *(location+1); value = value<<8;
	value |= *(location+2); value = value<<8;
	value |= *(location+3);
	return *((float*) &value);
}

static unsigned int sdf_read_unsigned_int(unsigned char* location)
{
	// <ZZ> This function reads an unsigned integer at the given memory location, assuming that
	//      the memory is stored in big endian format.  The function returns the value it
	//      read.
	unsigned int value;
	value  = *location;     value = value<<8;
	value |= *(location+1); value = value<<8;
	value |= *(location+2); value = value<<8;
	value |= *(location+3);
	return value;
}

static void sdf_write_unsigned_int(unsigned char* location, unsigned int value)
{
	// <ZZ> This function writes an unsigned integer to the given memory location, assuming that
	//      the memory is stored in big endian format.
	*(location+3) = (unsigned char) (value & 255);  value = value>>8;
	*(location+2) = (unsigned char) (value & 255);  value = value>>8;
	*(location+1) = (unsigned char) (value & 255);  value = value>>8;
	*(location)   = (unsigned char) (value & 255);
}

static unsigned short sdf_read_unsigned_short(unsigned char* location)
{
	// <ZZ> This function reads an unsigned short at the given memory location, assuming that
	//      the memory is stored in big endian format.  The function returns the value it
	//      read.
	unsigned short value;
	value  = *location;    value = value<<8;
	value |= *(location+1);
	return value;
}

static void sdf_write_unsigned_short(unsigned char* location, unsigned short value)
{
	// <ZZ> This function writes an unsigned short to the given memory location, assuming that
	//      the memory is stored in big endian format.
	*(location+1) = (unsigned char) (value & 255);  value = value>>8;
	*(location)   = (unsigned char) (value & 255);
}

static int count_indentation(char *string)
{
	// <ZZ> This function returns the number of spaces at the start of string
	int count;

	count = 0;
	while(string[count] == ' ')
	{
		count++;
	}
	return count;
}

static signed char src_read_token(unsigned char* buffer)
{
	// <ZZ> This function reads the next token (like a word) in a file that has been
	//      sdf_open()'d and puts that token in buffer.  The token can't be any longer than
	//      SRC_MAX_TOKEN_SIZE.  It returns TRUE if there was a token to read, or FALSE if
	//      not.  next_token_may_be_negative is also important for reading negative numbers
	//      correctly.
	int count;
	int value;


	// Skip any whitespace...
	while((*sdf_read_file) == ' ' && sdf_read_remaining > 0)
	{
		sdf_read_remaining--;
		sdf_read_file++;
	}


	// Check for a string token...
	count = 0;
	if((*sdf_read_file) == '\'' && (*(sdf_read_file+2)) == '\'')
	{
		sprintf(buffer, "%d", *(sdf_read_file+1));
		sdf_read_remaining-=3;
		sdf_read_file+=3;
		count=3;
	}
	else if((*sdf_read_file) == '"')
	{
		// Read the string token
		buffer[count] = '"';
		sdf_read_remaining--;
		sdf_read_file++;
		count++;
		while(count < SRC_MAX_TOKEN_SIZE-2 && sdf_read_remaining > 0 && (*sdf_read_file) != '"' && (*sdf_read_file) != 0 && (*sdf_read_file) != '\n')
		{
			buffer[count] = (*sdf_read_file);
			sdf_read_remaining--;
			sdf_read_file++;
			if(buffer[count] == '\\' && (*sdf_read_file) >= '0' && (*sdf_read_file) <= '9')
			{
				// It's an escape sequence, so read in the value...
				value = 0;
				while((*sdf_read_file) >= '0' && (*sdf_read_file) <= '9')
				{
					value = (value*10) + (*sdf_read_file) - '0';
					sdf_read_remaining--;
					sdf_read_file++;
				}
				buffer[count] = value;
			}
			count++;
		}
		buffer[count] = '"';
		sdf_read_remaining--;
		sdf_read_file++;
		count++;
		buffer[count] = 0;
		next_token_may_be_negative = FALSE;
	}
	else
	{
		// Is the token alphanumeric or a special operator token???
		if((((*sdf_read_file) >= 'A' && (*sdf_read_file) <= 'Z') || ((*sdf_read_file) >= 'a' && (*sdf_read_file) <= 'z') || (*sdf_read_file) == '_' || (*sdf_read_file) == '.' || ((*sdf_read_file) >= '0' && (*sdf_read_file) <= '9') ))
		{
			// Read the alphanumeric token
			while(count < SRC_MAX_TOKEN_SIZE-1 && sdf_read_remaining > 0 && ((((*sdf_read_file) >= 'A' && (*sdf_read_file) <= 'Z') || ((*sdf_read_file) >= 'a' && (*sdf_read_file) <= 'z') || (*sdf_read_file) == '_' || (*sdf_read_file) == '.' || ((*sdf_read_file) >= '0' && (*sdf_read_file) <= '9') )))
			{
				buffer[count] = (*sdf_read_file);
				sdf_read_remaining--;
				sdf_read_file++;
				count++;
			}
			buffer[count] = 0;
			make_uppercase(buffer);
			next_token_may_be_negative = FALSE;
		}
		else
		{
			// Read one character...
			if((*sdf_read_file) != 0 && (*sdf_read_file) != '\n')
			{
				if(sdf_read_remaining > 0)
				{
					buffer[count] = (*sdf_read_file);
					sdf_read_remaining--;
					sdf_read_file++;
					count++;
				}


				// Do we need another?
				if((buffer[0] == '>' && (*sdf_read_file) == '>') ||
				   (buffer[0] == '<' && (*sdf_read_file) == '<') ||
				   (buffer[0] == '&' && (*sdf_read_file) == '&') ||
				   (buffer[0] == '|' && (*sdf_read_file) == '|') ||
				   (buffer[0] == '>' && (*sdf_read_file) == '=') ||
				   (buffer[0] == '<' && (*sdf_read_file) == '=') ||
				   (buffer[0] == '=' && (*sdf_read_file) == '=') ||
				   (buffer[0] == '+' && (*sdf_read_file) == '+') ||
				   (buffer[0] == '-' && (*sdf_read_file) == '-') ||
				   (buffer[0] == '/' && (*sdf_read_file) == '/') ||
				   (buffer[0] == '!' && (*sdf_read_file) == '='))
				{
					buffer[count] = (*sdf_read_file);
					sdf_read_remaining--;
					sdf_read_file++;
					count++;
				}


				// Ignore comments...
				if(buffer[0] == '/' && buffer[1] == '/') return FALSE;



				// May need to read a whole number in...  Negative numbers...
				if(next_token_may_be_negative)
				{
					if(buffer[0] == '-' && (*sdf_read_file) >= '0' && (*sdf_read_file) <= '9')
					{
						// Read the numeric token...
						while(count < SRC_MAX_TOKEN_SIZE-1 && sdf_read_remaining > 0 && (((*sdf_read_file) == '.' || ((*sdf_read_file) >= '0' && (*sdf_read_file) <= '9'))))
						{
							buffer[count] = (*sdf_read_file);
							sdf_read_remaining--;
							sdf_read_file++;
							count++;
						}
						next_token_may_be_negative = FALSE;
					}
				}
				else
				{
					next_token_may_be_negative = TRUE;
				}
				buffer[count] = 0;



				// Figure out if the next one can be negative...
				if(strcmp(buffer, ")") == 0) next_token_may_be_negative = FALSE;
				if(strcmp(buffer, "--") == 0) next_token_may_be_negative = FALSE;
				if(strcmp(buffer, "++") == 0) next_token_may_be_negative = FALSE;
				if(strcmp(buffer, "%") == 0) next_token_may_be_negative = FALSE;
			}
		}
	}

	if(count > 0) return TRUE;
	return FALSE;
}

static signed char sdf_read_line(void)
{
	// <ZZ> This function places sdf_read_file at the start of the next good line in a file
	//      opened by sdf_open().  If something goes wrong, it returns FALSE.  If something
	//      goes right, it returns TRUE.


	// Make sure we have an open file
	if(sdf_read_file)
	{
		// Don't advance the read head on the first call...
		if(sdf_read_first_line == FALSE)
		{
			sdf_read_first_line = TRUE;
		}
		else
		{
			// Skip over the line until we hit a 0 or '\n'
			while((*sdf_read_file) != 0 &&
				(*sdf_read_file) != '\n' &&
				sdf_read_remaining > 0)
			{
				sdf_read_remaining--;
				sdf_read_file++;
			}
		}


		// Skip any 0's or \n's, so sdf_read_file starts on some good characters...
		while(((*sdf_read_file) == 0 ||
				(*sdf_read_file) == '\n') &&
			sdf_read_remaining > 0)
		{
			sdf_read_remaining--;
			sdf_read_file++;
			sdf_read_line_number++;
		}


		// Make sure we didn't run out of data...
		if(sdf_read_remaining) return TRUE;
	}
	return FALSE;
}

static void src_add_define(char* token, char* value, char temporary_level)
{
	// <ZZ> This function registers a new #define'd value.  Temporary_level determines if the
	//      define is removed after a file or function is done being parsed.
	char temptoken[SRC_MAX_TOKEN_SIZE];
	unsigned char* tempptr;
	int tempint;
	int tempnum;
	int i;
	int j;
	int found;
	int define_to_use;
	char define_is_new;


	// Look for an unused define to use...
	next_token_may_be_negative = TRUE;
	define_is_new = TRUE;
	define_to_use = src_num_define;
	repeat(i, src_num_define)
	{
		// Has this one been cleared?
		if(define_token[i][0] == 0)
		{
			define_to_use = i;
			define_is_new = FALSE;
			i = src_num_define;
		}
	}



	// Fill in the data
	if(define_to_use < SRC_MAX_DEFINE)
	{
		// Skip whitespace...
		while((*token) == ' ')  token++;
		while((*value) == ' ')  value++;



		// Copy the token...
		i = 0;
		while(i < SRC_MAX_TOKEN_SIZE-1 && token[i] != 0)
		{
			define_token[define_to_use][i] = token[i];
			i++;
		}
		define_token[define_to_use][i] = 0;
		define_temporary_level[define_to_use] = temporary_level;



		// Now search through the value, looking for previously #defined tokens...
		tempptr = sdf_read_file;
		tempint = sdf_read_remaining;
		tempnum = sdf_read_line_number;
		sdf_read_file = value;
		sdf_read_remaining = SRC_MAX_TOKEN_SIZE;


		i = 0;
		while(src_read_token(temptoken))
		{
			found = src_get_define(temptoken);
			if(found > -1)
			{
				// Copy the value of the old token into the new one...
				j = 0;
				while(define_value[found][j] != 0 && i < SRC_MAX_TOKEN_SIZE-1)
				{
					define_value[define_to_use][i] = define_value[found][j];
					i++;
					j++;
				}
				if(i < SRC_MAX_TOKEN_SIZE-1)
				{
					define_value[define_to_use][i] = ' ';
					i++;
				}
			}
			else
			{
				// Copy the token into the new define...
				j = 0;
				while(temptoken[j] != 0 && i < SRC_MAX_TOKEN_SIZE-1)
				{
					define_value[define_to_use][i] = temptoken[j];
					i++;
					j++;
				}
				if(i < SRC_MAX_TOKEN_SIZE-1)
				{
					define_value[define_to_use][i] = ' ';
					i++;
				}
			}
		}
		if(i < SRC_MAX_TOKEN_SIZE)
		{
			define_value[define_to_use][i] = 0;
			i--;
		}
		if(i >= 0)
		{
			if(define_value[define_to_use][i] == ' ')
			{
				define_value[define_to_use][i] = 0;
			}
		}
		sdf_read_line_number = tempnum;
		sdf_read_remaining = tempint;
		sdf_read_file = tempptr;


		//debug("INFO:         Defined...  %s == %s\n", define_token[define_to_use], define_value[define_to_use]);


		// Increment the number of defines if we added a new one
		if(define_is_new) src_num_define++;
	}
	else
	{
		debug("Error:  Couldn't define %s...  Out of tokens to use...\n", token);
		error = SSE_EXTERNAL;
	}
}

static int src_get_define(char* token)
{
	// <ZZ> This function returns the index of the first #define that matches token.  If there
	//      aren't any matches, it returns -1.

	// Check each define...
	for (int i = 0; i < src_num_define; ++i)
	{
		if(strcmp(token, define_token[i]) == 0) return i;
	}
	return -1;
}

static void make_uppercase(char *string)
{
	// <ZZ> This function changes all lowercase letters in string to be uppercase.
	int i;

	i = 0;
	while(string[i] != 0)
	{
		if(string[i] >= 'a' && string[i] <= 'z')  string[i] += 'A'-'a';
		i++;
	}
}

int obj_get_property(char* token)
{
	// <ZZ> This function returns the index of the first property that matches token.  If there
	//      aren't any matches, it returns -1.
	int i;

	// Check each define...
	repeat(i, obj_num_property)
	{
		if(strcmp(token, property_token[i]) == 0) return i;
	}
	return -1;
}

void src_close_jumps(unsigned char indent, unsigned char last_indent)
{
	// <ZZ> This function closes all of the if's and else's and while's down to the given
	//      indent level.
	int i;
	int j;


	i = last_indent;
	while(i > indent)
	{
		i--;
		if(i != 0)
		{
			if(last_jump_type_found[i] == SRC_JUMP_ELSE)
			{
				#ifdef VERBOSE_COMPILE
					log_message("INFO:   ELSE SKIPPER JUMPS TO HERE 0x%x", src_buffer_used);
				#endif
				sdf_write_unsigned_short(src_buffer+put_jump_offset_here[i], src_buffer_used);
			}
			else if(last_jump_type_found[i] == SRC_JUMP_IF)
			{
				// Insert implied jumps in order to skip over elses
				j = put_jump_offset_here[i];
				if(strcmp(token_buffer[0], "ELSE") == 0 && i == indent)
				{
					#ifdef VERBOSE_COMPILE
						log_message("INFO:         OPC:  0x%02x", OPCODE_JUMP);
						log_message("INFO:               0x%02x", 0);
						log_message("INFO:               0x%02x", 0);
					#endif
					src_buffer[src_buffer_used] = OPCODE_JUMP;  src_buffer_used++;
					sdf_write_unsigned_short(src_buffer+src_buffer_used, 0);
					put_jump_offset_here[i] = src_buffer_used;
					src_buffer_used+=2;
					last_jump_type_found[i] = SRC_JUMP_ELSE;
				}
				#ifdef VERBOSE_COMPILE
					log_message("INFO:   FAILED IF JUMPS TO HERE 0x%x", src_buffer_used);
				#endif
				sdf_write_unsigned_short(src_buffer+j, src_buffer_used);
			}
			else if(last_jump_type_found[i] == SRC_JUMP_WHILE)
			{
				#ifdef VERBOSE_COMPILE
					log_message("INFO:   JUMP BACK TO START OF WHILE 0x%x", while_jumps_back_to_here[i]);
					log_message("INFO:         OPC:  0x%02x", OPCODE_JUMP);
					log_message("INFO:               0x%02x", 0);
					log_message("INFO:               0x%02x", 0);
				#endif
				src_buffer[src_buffer_used] = OPCODE_JUMP;  src_buffer_used++;
				sdf_write_unsigned_short(src_buffer+src_buffer_used, while_jumps_back_to_here[i]);
				src_buffer_used+=2;
				#ifdef VERBOSE_COMPILE
					log_message("INFO:   FAILED WHILE JUMPS TO HERE 0x%x", src_buffer_used);
				#endif
				sdf_write_unsigned_short(src_buffer+put_jump_offset_here[i], src_buffer_used);
			}
			else
			  last_jump_type_found[i] = SRC_JUMP_INVALID;
		}
	}
}

void src_add_return_opcode(void)
{
	// <ZZ> This function appends a return opcode to the src_buffer.  Done to make sure
	//      function calls go back to where they came from.
	if(last_function_returns_integer)
	{
		#ifdef VERBOSE_COMPILE
			log_message("INFO:         OPC:  0x%02x", 225);         // Opcode for integer 1 is 225
			log_message("INFO:         OPC:  0x%02x", OPCODE_RETURNINT);
		#endif
		src_buffer[src_buffer_used] = 225;  src_buffer_used++;
		src_buffer[src_buffer_used] = OPCODE_RETURNINT;  src_buffer_used++;
	}
	else
	{
		#ifdef VERBOSE_COMPILE
			log_message("INFO:         OPC:  0x%02x", 226);         // Opcode for float 1.0 is 226
			log_message("INFO:         OPC:  0x%02x", OPCODE_RETURNFLOAT);
		#endif
		src_buffer[src_buffer_used] = 226;  src_buffer_used++;
		src_buffer[src_buffer_used] = OPCODE_RETURNFLOAT;  src_buffer_used++;
	}
}

int src_find_function_entry(unsigned char* filedata, char* functionname)
{
	// <ZZ> This function searches a .RUN file's header for a given function, and returns
	//      the function's entry (offset in the header) if it finds a match.  If there weren't
	//      any matches, it returns 0.  Functionname should be all uppercase...
	int number_of_functions;
	int i;
	unsigned char* filedatastart;

	// For each function in the header...
	filedatastart = filedata;
	filedata += MAX_FAST_FUNCTION << 1;  // Skip the fast function lookups...
	number_of_functions = sdf_read_unsigned_short(filedata);
	filedata += 4;
	repeat(i, number_of_functions)
	{
		// Check for a match...
		if (strcmp(functionname, (filedata+2)) == 0)
		{
			return (filedata-filedatastart);
		}
		// Go to the next entry
		filedata += 2;                    // Skip the address
		filedata += strlen(filedata)+1;   // Skip the name and 0
		filedata += strlen(filedata)+1;   // Skip the return and any arguments and 0
	}
	// Didn't find a match
	return 0;
}

void src_make_arrays(int token_count, char* filename)
{
	// <ZZ> This function helps the RPN code figure out what a given token is.  It breaks tokens
	//      into operands, functions, and operators (filling in the corresponding flag arrays).
	int i;
	int j;
	int length;
	char* dotread;


	// Clear out the order of operation for Reverse Polish Notation
	line_is_a_conditional = FALSE;
	repeat(i, token_count)
	{
		token_priority[i] = 0;
		token_variable_type[i] = VAR_INVALID;
		token_change_type[i] = VAR_INVALID;
		token_arg_list[i] = NULL;
		token_is_string[i] = FALSE;
	}


	// Figure out the opcode for each token, and what type of token it is...
	repeat(i, token_count)
	{
		token_opcode[i] = OPCODE_CALLFUNCTION;
		token_extension[i] = -1;
		token_number_to_destroy[i] = 0;
		if(strcmp(token_buffer[i], "=") == 0)   { token_opcode[i] =  OPCODE_EQUALS;         token_number_to_destroy[i] = 1; }
		if(strcmp(token_buffer[i], "+") == 0)   { token_opcode[i] =  OPCODE_ADD;            token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "-") == 0)   { token_opcode[i] =  OPCODE_SUBTRACT;       token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "*") == 0)   { token_opcode[i] =  OPCODE_MULTIPLY;       token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "/") == 0)   { token_opcode[i] =  OPCODE_DIVIDE;         token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "++") == 0)  { token_opcode[i] =  OPCODE_INCREMENT;      token_number_to_destroy[i] = 1; }
		if(strcmp(token_buffer[i], "--") == 0)  { token_opcode[i] =  OPCODE_DECREMENT;      token_number_to_destroy[i] = 1; }
		if(strcmp(token_buffer[i], "==") == 0)  { token_opcode[i] =  OPCODE_ISEQUAL;        token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "!=") == 0)  { token_opcode[i] =  OPCODE_ISNOTEQUAL;     token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], ">=") == 0)  { token_opcode[i] =  OPCODE_ISGREATEREQUAL; token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "<=") == 0)  { token_opcode[i] =  OPCODE_ISLESSEREQUAL;  token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], ">") == 0)   { token_opcode[i] =  OPCODE_ISGREATER;      token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "<") == 0)   { token_opcode[i] =  OPCODE_ISLESSER;       token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "&&") == 0)  { token_opcode[i] =  OPCODE_LOGICALAND;     token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "||") == 0)  { token_opcode[i] =  OPCODE_LOGICALOR;      token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "!") == 0)   { token_opcode[i] =  OPCODE_LOGICALNOT;     token_number_to_destroy[i] = 1; }
		if(strcmp(token_buffer[i], "&") == 0)   { token_opcode[i] =  OPCODE_BITWISEAND;     token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "|") == 0)   { token_opcode[i] =  OPCODE_BITWISEOR;      token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "<<") == 0)  { token_opcode[i] =  OPCODE_BITWISELEFT;    token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], ">>") == 0)  { token_opcode[i] =  OPCODE_BITWISERIGHT;   token_number_to_destroy[i] = 2; }
		if(strcmp(token_buffer[i], "%") == 0)   { token_opcode[i] =  OPCODE_MODULUS;        token_number_to_destroy[i] = 2; }


		// Subtract and Negate use the same symbol, '-', so see if it's a Negate...
		if(token_opcode[i] == OPCODE_SUBTRACT)
		{
			// Check the value to the left...
			if(i > 0)
			{
				if(token_is_operand[i-1] == FALSE && token_buffer[i-1][0] != ')')
				{
					// It's a negate...
					token_opcode[i] = OPCODE_NEGATE;
					token_number_to_destroy[i] = 1;
				}
			}
		}


		// We should now know if it's an operator or not...
		token_is_operator[i] = (token_opcode[i] != OPCODE_CALLFUNCTION);


		// Are we still looking?
		if(token_opcode[i] == OPCODE_CALLFUNCTION)
		{
			// Yup...  Maybe it's an integer or a float...
			if(token_buffer[i][0] == 'I')
			{
				j = -1;
				if(token_buffer[i][1] >= '0' && token_buffer[i][1] <= '9')
				{
					j = token_buffer[i][1] - '0';
					if(token_buffer[i][2] >= '0' && token_buffer[i][2] <= '9')
					{
						j = j * 10;
						j += token_buffer[i][2] - '0';
					}
					if(token_buffer[i][2] == '.')
					{
						sscanf(token_buffer[i]+3, "%d", &token_extension[i]);
					}
					if(token_buffer[i][3] == '.' && token_buffer[i][2] != 0)
					{
						sscanf(token_buffer[i]+4, "%d", &token_extension[i]);
					}
					token_variable_type[i] = VAR_INT;
				}
				if(j >= 0 && j < MAX_VARIABLE) token_opcode[i] = 128 + j;
				if(token_extension[i] > -1)
				{
					// It's a property...  Set the variable type correctly...
					token_opcode[i] += 32;
					if(property_type[token_extension[i]] == 'F')
					{
						token_variable_type[i] = VAR_FLOAT;
					}
				}
				token_number_to_destroy[i] = -1;
			}
			if(token_buffer[i][0] == 'F')
			{
				j = -1;
				if(token_buffer[i][1] >= '0' && token_buffer[i][1] <= '9')
				{
					j = token_buffer[i][1] - '0';
					if(token_buffer[i][2] >= '0' && token_buffer[i][2] <= '9')
					{
						j = j * 10;
						j += token_buffer[i][2] - '0';
					}
					token_variable_type[i] = VAR_FLOAT;
				}
				if(j >= 0 && j < MAX_VARIABLE) token_opcode[i] = 192 + j;
				token_number_to_destroy[i] = -1;
			}
		}


		// Are we still looking?
		if(token_opcode[i] == OPCODE_CALLFUNCTION)
		{
			// Yup...  Maybe it's a numerical token...
			if((token_buffer[i][0] >= '0' && token_buffer[i][0] <= '9') || token_buffer[i][0] == '-')
			{
				// Does it have a dot?
				dotread = strpbrk(token_buffer[i], ".");
				if(dotread != NULL)
				{
					// Treat it as a numerical float
					sscanf(token_buffer[i], "%f", (float *) (&token_extension[i]));
					token_opcode[i] = 230;
					if(strcmp(token_buffer[i], "1.0") == 0)  token_opcode[i] = 226;
					if(strcmp(token_buffer[i], "1.00") == 0)  token_opcode[i] = 226;
					if(strcmp(token_buffer[i], "1.000") == 0)  token_opcode[i] = 226;
					if(strcmp(token_buffer[i], "1.0000") == 0)  token_opcode[i] = 226;
					token_variable_type[i] = VAR_FLOAT;
				}
				else
				{
					// Treat it as a numerical integer
					sscanf(token_buffer[i], "%d", &token_extension[i]);
					token_opcode[i] = 228;
					if(token_extension[i] >= -128 && token_extension[i] <= 127) token_opcode[i] = 227;
					if(token_extension[i] < -32768 || token_extension[i] > 32767) token_opcode[i] = 229;
					if(token_extension[i] == 0)  token_opcode[i] = 224;
					if(token_extension[i] == 1)  token_opcode[i] = 225;
					token_variable_type[i] = VAR_INT;
				}
				token_number_to_destroy[i] = -1;
			}
		}


		// Are we still looking?
		if(token_opcode[i] == OPCODE_CALLFUNCTION)
		{
			// Yup...  Maybe it's a string...  Treat as a 4 byte integer...
			if(token_buffer[i][0] == '"')
			{
				// Cut of the last " mark...
				length = strlen(token_buffer[i]+1);
				token_buffer[i][length] = 0;

				// Figure out the offset of the string in memory...  2 to skip address in header...
				token_extension[i] = src_find_string_entry(src_buffer, token_buffer[i]+1)+2;
				token_variable_type[i] = VAR_INT;


				// Need to go back through these during functionize...
				// ...But in order to do that, we need to remember where the opcode is...
				// ...But we don't know where it is yet, so we'll have to wait 'til later...
				token_is_string[i] = TRUE;
				token_opcode[i] = 229;


				// Put the " mark back on...  Just in case there's something weird I forgot about...
				token_buffer[i][length] = '"';
				token_number_to_destroy[i] = -1;
			}
		}


		// We should now know if it's a function or not...
		token_is_function[i] = (token_opcode[i] == OPCODE_CALLFUNCTION);


		// We should now know if it's an operand or not...
		token_is_operand[i] = 1 - token_is_function[i] - token_is_operator[i];
		if(token_buffer[i][0] == ',' || token_buffer[i][0] == '(' || token_buffer[i][0] == ')')
		{
			token_is_operator[i] = FALSE;
			token_is_function[i] = FALSE;
			token_is_operand[i] = FALSE;
			token_variable_type[i] = VAR_INVALID;
			token_number_to_destroy[i] = 0;
		}


		// Are we still looking?
		if(token_is_function[i])
		{
			// Yup...  Maybe it's a basic function...
			if(strcmp(token_buffer[i], "IF") == 0)      { token_opcode[i] = OPCODE_IFFALSEJUMP;  token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT;  line_is_a_conditional = TRUE; }
			if(strcmp(token_buffer[i], "WHILE") == 0)   { token_opcode[i] = OPCODE_IFFALSEJUMP;  token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT;  line_is_a_conditional = TRUE; }
			if(strcmp(token_buffer[i], "ELSE") == 0)    { line_is_a_conditional = TRUE; }
			if(strcmp(token_buffer[i], "TOINT") == 0)   { token_opcode[i] = OPCODE_TOINT;        token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_float;    token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "TOFLOAT") == 0) { token_opcode[i] = OPCODE_TOFLOAT;      token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_FLOAT; }
			if(strcmp(token_buffer[i], "LOCALMESSAGE")==0){token_opcode[i]= OPCODE_LOCALMESSAGE; token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "NETWORKMESSAGE")==0){token_opcode[i]= OPCODE_NETWORKMESSAGE; token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "LOGMESSAGE")==0){ token_opcode[i] = OPCODE_LOGMESSAGE;   token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "FINDSELF") == 0){ token_opcode[i] = OPCODE_FINDSELF;     token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "SYSTEMSET") ==0){ token_opcode[i] = OPCODE_SYSTEMSET;    token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_iiii;     token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "SYSTEMGET") ==0){ token_opcode[i] = OPCODE_SYSTEMGET;    token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "DEBUGMESSAGE")==0){token_opcode[i]= OPCODE_DEBUGMESSAGE; token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "SQRT") == 0)    { token_opcode[i] = OPCODE_SQRT;         token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_float;    token_variable_type[i] = VAR_FLOAT; }
			if(strcmp(token_buffer[i], "FILEOPEN")==0)  { token_opcode[i] = OPCODE_FILEOPEN;     token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "FILEREADBYTE")==0){token_opcode[i]= OPCODE_FILEREADBYTE; token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "FILEWRITEBYTE")==0){token_opcode[i]=OPCODE_FILEWRITEBYTE;token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "FILEINSERT")==0){ token_opcode[i] = OPCODE_FILEINSERT;   token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_iiii;     token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "SPAWN") == 0)   { token_opcode[i] = OPCODE_SPAWN;        token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_ifffi;    token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "GOPOOF") == 0)  { token_opcode[i] = OPCODE_GOPOOF;       token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int; }
			if(strcmp(token_buffer[i], "DISMOUNT") == 0){ token_opcode[i] = OPCODE_DISMOUNT;     token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none; }
			if(strcmp(token_buffer[i], "ROLLDICE") ==0) { token_opcode[i] = OPCODE_ROLLDICE;     token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "PLAYSOUND") ==0){ token_opcode[i] = OPCODE_PLAYSOUND;    token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "PLAYMEGASOUND")==0){token_opcode[i]=OPCODE_PLAYMEGASOUND;token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_iiiii;    token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "DISTANCESOUND")==0){token_opcode[i]=OPCODE_DISTANCESOUND;token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "PLAYMUSIC") ==0){ token_opcode[i] = OPCODE_PLAYMUSIC;    token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "UPDATEFILES")==0){token_opcode[i] = OPCODE_UPDATEFILES;  token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;}
			if(strcmp(token_buffer[i], "SIN")==0)       { token_opcode[i] = OPCODE_SIN;          token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_float;    token_variable_type[i] = VAR_FLOAT; }


			// String functions...
			if(strcmp(token_buffer[i], "STRING") == 0)            { token_opcode[i] = OPCODE_STRING;             token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGGETNUMBER") == 0)   { token_opcode[i] = OPCODE_STRINGGETNUMBER;    token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGCLEAR") == 0)       { token_opcode[i] = OPCODE_STRINGCLEAR;        token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGCLEARALL") == 0)    { token_opcode[i] = OPCODE_STRINGCLEARALL;     token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGAPPEND") == 0)      { token_opcode[i] = OPCODE_STRINGAPPEND;       token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGCOMPARE") == 0)     { token_opcode[i] = OPCODE_STRINGCOMPARE;      token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGLENGTH") == 0)      { token_opcode[i] = OPCODE_STRINGLENGTH;       token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGCHOPLEFT") == 0)    { token_opcode[i] = OPCODE_STRINGCHOPLEFT;     token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGCHOPRIGHT") == 0)   { token_opcode[i] = OPCODE_STRINGCHOPRIGHT;    token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGGETVALUE") == 0)    { token_opcode[i] = OPCODE_FILEREADBYTE;       token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }  // StringGetValue is the same as FileReadByte...
			if(strcmp(token_buffer[i], "STRINGSETVALUE") == 0)    { token_opcode[i] = OPCODE_FILEWRITEBYTE;      token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }  // StringSetValue is the same as FileWriteByte...
			if(strcmp(token_buffer[i], "STRINGRANDOMNAME") == 0)  { token_opcode[i] = OPCODE_STRINGRANDOMNAME;   token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGSANITIZE") == 0)    { token_opcode[i] = OPCODE_STRINGSANITIZE;     token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGLANGUAGE") == 0)    { token_opcode[i] = OPCODE_STRINGLANGUAGE;     token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGUPPERCASE") == 0)   { token_opcode[i] = OPCODE_STRINGUPPERCASE;    token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "STRINGAPPENDNUMBER") == 0){ token_opcode[i] = OPCODE_STRINGAPPENDNUMBER; token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii;      token_variable_type[i] = VAR_INT; }


			// Window functions
			if(strcmp(token_buffer[i], "WINDOWBORDER") == 0)       { token_opcode[i] = OPCODE_WINDOWBORDER;      token_number_to_destroy[i] = 6; token_arg_list[i] = arg_list_iffiii;   token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWSTRING") == 0)       { token_opcode[i] = OPCODE_WINDOWSTRING;      token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_iffi;     token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWMINILIST") == 0)     { token_opcode[i] = OPCODE_WINDOWMINILIST;    token_number_to_destroy[i] = 6; token_arg_list[i] = arg_list_ffiiii;   token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWSLIDER") == 0)       { token_opcode[i] = OPCODE_WINDOWSLIDER;      token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_ffiii;    token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWIMAGE") == 0)        { token_opcode[i] = OPCODE_WINDOWIMAGE;       token_number_to_destroy[i] = 7; token_arg_list[i] = arg_list_ffffiii;  token_variable_type[i] = VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWTRACKER") == 0)      { token_opcode[i] = OPCODE_WINDOWTRACKER;     token_number_to_destroy[i] = 10;token_arg_list[i] = arg_list_ffffiiiiii;token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWBOOK") == 0)         { token_opcode[i] = OPCODE_WINDOWBOOK;        token_number_to_destroy[i] = 7; token_arg_list[i] = arg_list_ffiiiii;  token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWINPUT") == 0)        { token_opcode[i] = OPCODE_WINDOWINPUT;       token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_ffiii;    token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWEMACS") == 0)        { token_opcode[i] = OPCODE_WINDOWEMACS;       token_number_to_destroy[i] = 6; token_arg_list[i] = arg_list_ffiiii;   token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWMEGAIMAGE") == 0)    { token_opcode[i] = OPCODE_WINDOWMEGAIMAGE;   token_number_to_destroy[i] = 15;token_arg_list[i] = arg_list_ffffffffffffiii; token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOW3DSTART") == 0)      { token_opcode[i] = OPCODE_WINDOW3DSTART;     token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_ffffi;    token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOW3DEND") == 0)        { token_opcode[i] = OPCODE_WINDOW3DEND;       token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOW3DPOSITION") == 0)   { token_opcode[i] = OPCODE_WINDOW3DPOSITION;  token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_fffi;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOW3DMODEL") == 0)      { token_opcode[i] = OPCODE_WINDOW3DMODEL;     token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_iiii;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "MODELASSIGN") == 0)        { token_opcode[i] = OPCODE_MODELASSIGN;       token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int; }
			if(strcmp(token_buffer[i], "PARTICLEBOUNCE") == 0)     { token_opcode[i] = OPCODE_PARTICLEBOUNCE;    token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "WINDOWEDITKANJI") == 0)    { token_opcode[i] = OPCODE_WINDOWEDITKANJI;   token_number_to_destroy[i] = 5; token_arg_list[i] = arg_list_ifffi; }
			if(strcmp(token_buffer[i], "WINDOW3DROOM") == 0)       { token_opcode[i] = OPCODE_WINDOW3DROOM;      token_number_to_destroy[i] = 7; token_arg_list[i] = arg_list_fffiiii; }


			// Other functions...
			if(strcmp(token_buffer[i], "INDEXISLOCALPLAYER") == 0) { token_opcode[i] = OPCODE_INDEXISLOCALPLAYER;token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDBINDING") == 0)        { token_opcode[i] = OPCODE_FINDBINDING;       token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDTARGET") == 0)         { token_opcode[i] = OPCODE_FINDTARGET;        token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDOWNER") == 0)          { token_opcode[i] = OPCODE_FINDOWNER;         token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDINDEX") == 0)          { token_opcode[i] = OPCODE_FINDINDEX;         token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDBYINDEX") == 0)        { token_opcode[i] = OPCODE_FINDBYINDEX;       token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDWINDOW") == 0)         { token_opcode[i] = OPCODE_FINDWINDOW;        token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_int_int;  token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDPARTICLE") == 0)       { token_opcode[i] = OPCODE_FINDPARTICLE;      token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "ATTACHTOTARGET") == 0)     { token_opcode[i] = OPCODE_ATTACHTOTARGET;    token_number_to_destroy[i] = 1; token_arg_list[i] = arg_list_int;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "GETDIRECTION") == 0)       { token_opcode[i] = OPCODE_GETDIRECTION;      token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_float_float; token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "DAMAGETARGET") == 0)       { token_opcode[i] = OPCODE_DAMAGETARGET;      token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii; }
			if(strcmp(token_buffer[i], "EXPERIENCEFUNCTION") == 0) { token_opcode[i] = OPCODE_EXPERIENCEFUNCTION;token_number_to_destroy[i] = 4; token_arg_list[i] = arg_list_iiii;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "ACQUIRETARGET") == 0)      { token_opcode[i] = OPCODE_ACQUIRETARGET;     token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iif;      token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "FINDPATH") == 0)           { token_opcode[i] = OPCODE_FINDPATH;          token_number_to_destroy[i] = 0; token_arg_list[i] = arg_list_none;     token_variable_type[i]= VAR_INT; }
			if(strcmp(token_buffer[i], "BUTTONPRESS") == 0)        { token_opcode[i] = OPCODE_BUTTONPRESS;       token_number_to_destroy[i] = 3; token_arg_list[i] = arg_list_iii; }
			if(strcmp(token_buffer[i], "AUTOAIM") == 0)            { token_opcode[i] = OPCODE_AUTOAIM;           token_number_to_destroy[i] = 7; token_arg_list[i] = arg_list_ffiiiii;  token_variable_type[i]= VAR_FLOAT; }
			if(strcmp(token_buffer[i], "ROOMHEIGHTXY") == 0)       { token_opcode[i] = OPCODE_ROOMHEIGHTXY;      token_number_to_destroy[i] = 2; token_arg_list[i] = arg_list_float_float;  token_variable_type[i]= VAR_FLOAT; }


			// Return function...
			if(strcmp(token_buffer[i], "RETURN") == 0)
			{
				token_number_to_destroy[i] = 1;
				if(last_function_returns_integer)     { token_opcode[i] = OPCODE_RETURNINT;     token_arg_list[i] = arg_list_int; }
				else                                  { token_opcode[i] = OPCODE_RETURNFLOAT;   token_arg_list[i] = arg_list_float; }
			}
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  More basic functions
// !!!BAD!!!
// !!!BAD!!!
		}


		// Or it might be a user function...
		if(token_is_function[i] && token_opcode[i] == OPCODE_CALLFUNCTION)
		{
			// Ignore else...
			if(strcmp(token_buffer[i], "ELSE") != 0)
			{
				// Search for the function, so we can figure out what arguments it requires...
				token_arg_list[i] = src_mega_find_function(token_buffer[i], filename, SRC_REQUEST_ARGUMENTS);
				if(token_arg_list[i])
				{
					token_variable_type[i] = token_arg_list[i][0];
					token_arg_list[i]++;  // Skip the return code
					token_number_to_destroy[i] = strlen(token_arg_list[i]);
				}
				else
				{
					// Function not found...
					debug("Error: Line %d, Unrecognized symbol %s found\n", sdf_read_line_number, token_buffer[i]);
					error = SSE_EXTERNAL;
				}
			}
		}
	}
}

void src_find_priority(int start, int end, signed char change_signs, signed char first_check)
{
	// <ZZ> This function looks at the tokens in token_buffer to determine the reverse polish
	//      ordering for the tokens.  Start is the first token to be examined, end is the last.
	//      If the expression is complex (as it usually is), it is cut in half at the lowest
	//      priority operator, and each half is fed back into the function recursively.  If the
	//      expression is simple enough to figure out, the priorities are assigned.
	int i;
	int priority;
	int lowest_token;
	int lowest_priority;
	int highest_priority;
	int nest_level;
	int our_nest_level;


	// Just in case something weird happens...
	if(end < start || start < 0 || start >= SRC_MAX_TOKEN || end < 0 || start >= SRC_MAX_TOKEN)
	{
		return;
	}


	// Change signs if negative...  Don't propogate through parentheses...
	if(change_signs)
	{
		nest_level = 0;
		i = start;
		while(i <= end)
		{
			if(token_buffer[i][0] == '(')
			{
				nest_level++;
			}
			if(token_buffer[i][0] == ')')
			{
				nest_level--;
			}
			if(nest_level == 0)
			{
				if(token_opcode[i] == OPCODE_ADD)
				{
					token_opcode[i] = OPCODE_SUBTRACT;
					token_buffer[i][0] = '-';
				}
				else if(token_opcode[i] == OPCODE_SUBTRACT)
				{
					token_opcode[i] = OPCODE_ADD;
					token_buffer[i][0] = '+';
				}
			}
			i++;
		}
	}



	// Draw in ends to ignore parentheses...
	while(token_buffer[start][0] == '(' && token_buffer[end][0] == ')')
	{
		start++;
		end--;
	}


	// Just in case something weird happens...
	if(end < start)
	{
		return;
	}

	#ifdef VERBOSE_COMPILE
		log_message("INFO:   Checking priority from %d to %d", start, end);
	#endif


	// Find the lowest nest level...
	our_nest_level = 0;
	nest_level = 0;
	i = start;
	while(i <= end)
	{
		// Check for nest level changes...
		if(token_buffer[i][0] == '(')
		{
			nest_level++;
		}
		if(token_buffer[i][0] == ')')
		{
			nest_level--;
		}
		if(token_buffer[i][0] != '(' && token_buffer[i][0] != ')')
		{
			// Find the first useable token...
			our_nest_level = nest_level;
			i = end;
		}
		i++;
	}
	nest_level = 0;
	i = start;
	while(i <= end)
	{
		// Check for nest level changes...
		if(token_buffer[i][0] == '(')
		{
			nest_level++;
		}
		if(token_buffer[i][0] == ')')
		{
			nest_level--;
		}
		if(token_buffer[i][0] != '(' && token_buffer[i][0] != ')')
		{
			// Only take nest levels that actually have tokens...
			if(nest_level < our_nest_level)  our_nest_level = nest_level;
		}
		i++;
	}







	// Search for the lowest and highest priority tokens
	lowest_token = -1;
	lowest_priority = 1000000;
	highest_priority = -1;
	nest_level = 0;
	i = start;
	while(i <= end)
	{
		// Check for nest level changes...
		if(token_buffer[i][0] == '(')
		{
			nest_level++;
			highest_priority = 100000;  // Force a split
		}
		if(token_buffer[i][0] == ')')
		{
			nest_level--;
		}
		// Only handle tokens at our nest level...
		else if(nest_level == our_nest_level)
		{
			priority = -1;

			// First come increment and decrement
			if(token_opcode[i] == OPCODE_INCREMENT || token_opcode[i] == OPCODE_DECREMENT)
			{
				priority = 9;
			}
			// Then not and negate...
			else if(token_opcode[i] == OPCODE_LOGICALNOT || token_opcode[i] == OPCODE_NEGATE)
			{
				priority = 8;
			}
			// Then function calls
			else if(token_is_function[i])
			{
				priority = 7;
			}
			// Then multiply, divide, and modulus
			else if(token_opcode[i] == OPCODE_MULTIPLY || token_opcode[i] == OPCODE_DIVIDE || token_opcode[i] == OPCODE_MODULUS)
			{
				priority = 6;
			}
			// Then add and subtract
			else if(token_opcode[i] == OPCODE_ADD || token_opcode[i] == OPCODE_SUBTRACT)
			{
				priority = 5;
			}
			// Then bitwise operations
			else if(token_opcode[i] >= OPCODE_BITWISEAND && token_opcode[i] <= OPCODE_BITWISERIGHT)
			{
				priority = 4;
			}
			// Then comparison operations
			else if(token_opcode[i] >= OPCODE_ISEQUAL && token_opcode[i] <= OPCODE_ISLESSER)
			{
				priority = 3;
			}
			// Then logical operations
			else if(token_opcode[i] == OPCODE_LOGICALAND || token_opcode[i] == OPCODE_LOGICALOR)
			{
				priority = 2;
			}
			// Then commas
			else if(token_buffer[i][0] == ',')
			{
				priority = 1;
			}
			// Then equates
			else if(token_opcode[i] == OPCODE_EQUALS)
			{
				priority = 0;
			}


			// Now remember the priority...
			if(priority > -1)
			{
				if(priority > highest_priority)
				{
					highest_priority = priority;
				}
				if(priority < lowest_priority || (priority == 6 && lowest_priority == 6))
				{
					lowest_token = i;
					lowest_priority = priority;
				}
			}
		}
		i++;
	}


	// Check for errors...
	if(nest_level != 0 && first_check)
	{
		debug("Error: Line %d, Unbalanced parentheses\n", sdf_read_line_number);
		error = SSE_EXTERNAL;
		return;
	}


	// Now, if the expression only contains tokens of one priority, we can solve it...
	if((highest_priority == lowest_priority && lowest_priority != 7 && lowest_priority != 0 && lowest_priority != 1) || lowest_token == -1)
	{
		// Solve the expression...
		i = start;
		while(i <= end)
		{
			if(token_is_operator[i])
			{
				if(token_opcode[i] == OPCODE_INCREMENT || token_opcode[i] == OPCODE_DECREMENT)
				{
					// Handle self
					src_set_priority(start, i, end, TRUE);

					// Handle operand to left
					src_set_priority(start, i-1, end, FALSE);
				}
				else
				{
					// Handle operand to left
					src_set_priority(start, i-1, end, FALSE);

					// Handle operand to right
					src_set_priority(start, i+1, end, FALSE);

					// Handle self
					src_set_priority(start, i, end, TRUE);
				}
			}
			else if(lowest_token == -1)
			{
				// Handle self
				src_set_priority(start, i, end, FALSE);
			}
			i++;
		}
	}
	else
	{
		// Too complex, so split it in two and try again...
		if(token_opcode[lowest_token] == OPCODE_EQUALS || token_is_function[lowest_token])
		{
			// Right side
			src_find_priority(lowest_token+1, end, FALSE, FALSE);

			// Middle
			src_set_priority(start, lowest_token, end, TRUE);

			// Left side
			src_find_priority(start, lowest_token-1, FALSE, FALSE);
		}
		else if(token_opcode[lowest_token] == OPCODE_LOGICALNOT || token_opcode[lowest_token] == OPCODE_NEGATE)
		{
			// Right side
			src_find_priority(lowest_token+1, end, FALSE, FALSE);

			// Middle
			src_set_priority(start, lowest_token, end, TRUE);
		}
		else
		{
			// Left side
			src_find_priority(start, lowest_token-1, FALSE, FALSE);

			// Right side
			if(token_opcode[lowest_token] == OPCODE_SUBTRACT) src_find_priority(lowest_token+1, end, TRUE, FALSE);
			else src_find_priority(lowest_token+1, end, FALSE, FALSE);

			// Middle
			src_set_priority(start, lowest_token, end, TRUE);
		}
	}
	return;
}

void src_figure_variable_types(int token_count)
{
	// <ZZ> This function determines where to place the ToInt and ToFloat opcodes for a line.
	//      Token_count is the number of RPN symbols found previously, which doesn't include
	//      tokens like '(' and ','.
	int i;
	int j;
	int destroyed;
	int total_destroyed;
	int token;
	int other_token;
	unsigned char type_needed;


	repeat(i, token_count)
	{
		// Look for tokens in order of their priority...
		token = token_priority_list[i];
		token_is_destroyed[token] = FALSE;


		// Look through previous tokens to find arguments for functions/operations
		type_needed = VAR_INT;
		if(token_number_to_destroy[token] > 0)
		{
			// Look for errors with equals, increment, and decrement
			if(token_variable_type[token] == VAR_INVALID)
			{
				if(token_opcode[token] == OPCODE_EQUALS || token_opcode[token] == OPCODE_INCREMENT || token_opcode[token] == OPCODE_DECREMENT)
				{
					// Need to know what type of variable is being set...  Check it...
					if(i < token_count)
					{
						// Make sure the token to the left is a valid data storage token...
						other_token = token_priority_list[i+1];
						if(token_opcode[other_token] >= 128 && token_opcode[other_token] < 224 )
						{
							if(token_opcode[token] == OPCODE_EQUALS)
							{
								// Check to the right for equals, now that we've checked for errors...
								if(i > 0)
								{
									other_token = token_priority_list[i-1];
									type_needed = token_variable_type[other_token];
								}
								else
								{
									debug("Error:  Line %d, Missing value to right of =\n", sdf_read_line_number);
									error = SSE_EXTERNAL;
								}
							}
							else
							{
								type_needed = token_variable_type[other_token];
							}
						}
						else
						{
							debug("Error:  Line %d, Invalid storage variable to left of =, ++, or, --\n", sdf_read_line_number);
							error = SSE_EXTERNAL;
						}
					}
				}
			}
			// Look for any floating point arguments...  This is needed for basic functions
			// and operations that can handle either float or integer...  If an Add has a float
			// and an int, the int should be changed to float and a FloatAdd performed...
			if(token_arg_list[token] == NULL)
			{
				destroyed = 0;
				j = i;
				while(j > 0 && destroyed < token_number_to_destroy[token])
				{
					// Go to the last token and see if we can use it...
					j--;
					other_token = token_priority_list[j];
					if(token_is_destroyed[other_token] == FALSE)
					{
						// Okay, we can use it...  Check if it's a float...
						if(token_variable_type[other_token] == VAR_FLOAT) type_needed = VAR_FLOAT;
						destroyed++;
					}
				}


				// Now change the opcodes from IntAdd to FloatAdd...  Others too...
				if(type_needed == VAR_FLOAT)
				{
					if(token_opcode[token] <= OPCODE_NEGATE)
					{
						token_opcode[token]+=32;  // Float operations start with opcode 32...
					}
				}
			}
			// Fill in the correct variable type...
			if(token_variable_type[token] == VAR_INVALID)
			{
				token_variable_type[token] = type_needed;
				// Some floating point functions return integers...
				if(token_opcode[token] >= OPCODE_F_ISEQUAL && token_opcode[token] <= OPCODE_F_LOGICALNOT)
				{
					token_variable_type[token] = VAR_INT;
				}
			}




			// Now type cast any of those arguments we found...  Do the search again...
			destroyed = 0;
			j = i;
			while(j > 0 && destroyed < token_number_to_destroy[token])
			{
				// Go to the last token and see if we can use it...
				j--;
				other_token = token_priority_list[j];
				if(token_is_destroyed[other_token] == FALSE)
				{
					// Remember that we ate it up, so other functions don't try to use it too...
					token_is_destroyed[other_token] = TRUE;
					destroyed++;


					// Okay, we can use it...  Do we need to change all ints to float?
					if(token_arg_list[token] != NULL)
					{
						// Looks like a function with a set argument list, so just cast any
						// argument we need to
						type_needed = *(token_arg_list[token]+token_number_to_destroy[token]-destroyed);
					}
					if(token_variable_type[other_token] != type_needed)
					{
						// Don't insert ToInt or ToFloat for equates...
						if(token_opcode[token] != OPCODE_EQUALS && token_opcode[token] != OPCODE_F_EQUALS)
						{
							token_change_type[other_token] = type_needed;
						}
					}
				}
			}
		}
		last_return_type = token_variable_type[token];
	}



	// Make sure we had the correct number of arguments
	total_destroyed = 0;
	repeat(i, token_count)
	{
		token = token_priority_list[i];
		if(token_opcode[token] == OPCODE_EQUALS || token_opcode[token] == OPCODE_F_EQUALS)  token_number_to_destroy[token]++;
		if(token_number_to_destroy[token] >= 0)  token_number_to_destroy[token]--;
		total_destroyed += token_number_to_destroy[token];
		#ifdef VERBOSE_COMPILE
			log_message("INFO:         RPN:  %s  DES:  %d", token_buffer[token], token_number_to_destroy[token]);
		#endif
	}
	#ifdef VERBOSE_COMPILE
		log_message("INFO:   ");
	#endif
	if(total_destroyed != -1)
	{
		debug("Error:  Line %d, Wrong number of arguments (%d to %d)\n", sdf_read_line_number, total_destroyed, -1);
		error = SSE_EXTERNAL;
	}


	// Debug stuff...
	#ifdef VERBOSE_COMPILE
		repeat(i, token_count)
		{
			token = token_priority_list[i];
			log_message("INFO:         TYP:  %c  CHG:  %c  RPN:  %s", token_variable_type[token], token_change_type[token], token_buffer[token]);
		}
		log_message("INFO:   ");
	#endif
}

void src_generate_opcodes(int token_count)
{
	// <ZZ> This function writes the finalized opcodes for a single line of a SRC file.  The
	//      opcodes are put in the src_buffer.  Token_count is the number of RPN symbols found
	//      previously, which doesn't include tokens like '(' and ','.
	int i;
	int token;
	unsigned int tempint;
	signed char last_token_was_equals;


	last_token_was_equals = FALSE;
	repeat(i, token_count)
	{
		// Write out the opcode and any extended data...
		token = token_priority_list[i];


		// ...But ignore else's...
		if(strcmp(token_buffer[token], "ELSE") != 0)
		{
			#ifdef VERBOSE_COMPILE
				log_message("INFO:         OPC:  0x%02x", token_opcode[token]);
			#endif
			src_buffer[src_buffer_used] = token_opcode[token];  src_buffer_used++;




			// Look for variables that are used before being set...
			if(last_token_was_equals)
			{
				// Variables being set...
				if(token_opcode[token] >= 128 && token_opcode[token] < 224)
				{
					if((token_opcode[token] & 224) == 192)
					{
						// Float variable has been set...
						float_variable_set[token_opcode[token] & (MAX_VARIABLE-1)] = TRUE;
					}
					if((token_opcode[token] & 224) == 128)
					{
						// Integer variable has been set...
						int_variable_set[token_opcode[token] & (MAX_VARIABLE-1)] = TRUE;
					}
				}
				else
				{
					log_message("ERROR:  Line %d, Can only set/assign variables", sdf_read_line_number);
				}
			}
			else
			{
				// Variables being used...
				if(token_opcode[token] >= 128 && token_opcode[token] < 224)
				{
					if((token_opcode[token] & 224) == 192)
					{
						if(float_variable_set[token_opcode[token] & (MAX_VARIABLE-1)] == FALSE)
						{
							// Variable has been used before being set...
							debug("ERROR:  Line %d, Variable F%02d used before being set\n", sdf_read_line_number, (token_opcode[token] & (MAX_VARIABLE-1)));
							error = SSE_EXTERNAL;
						}
					}
					else
					{
						if(int_variable_set[token_opcode[token] & (MAX_VARIABLE-1)] == FALSE)
						{
							// Variable has been used before being set...
							debug("ERROR:  Line %d, Variable I%02d used before being set\n", sdf_read_line_number, (token_opcode[token] & (MAX_VARIABLE-1)));
							error = SSE_EXTERNAL;
						}
					}
				}
			}
			last_token_was_equals = FALSE;
			if(token_opcode[token] == OPCODE_EQUALS || token_opcode[token] == OPCODE_F_EQUALS)
			{
				last_token_was_equals = TRUE;
			}



			if(token_opcode[token] == OPCODE_CALLFUNCTION)
			{
				// Call function...  4 byte jump location, 4 byte return
				// position, 4 byte argument list pointer, 4 byte file start address,
				// then the zero terminated function name
				#ifdef VERBOSE_COMPILE
					log_message("INFO:            :  0x00 (jump)");
					log_message("INFO:            :  0x00 (jump)");
					log_message("INFO:            :  0x00 (jump)");
					log_message("INFO:            :  0x00 (jump)");
					log_message("INFO:            :  0x00 (return)");
					log_message("INFO:            :  0x00 (return)");
					log_message("INFO:            :  0x00 (return)");
					log_message("INFO:            :  0x00 (return)");
					log_message("INFO:            :  0x00 (arg)");
					log_message("INFO:            :  0x00 (arg)");
					log_message("INFO:            :  0x00 (arg)");
					log_message("INFO:            :  0x00 (arg)");
					log_message("INFO:            :  0x00 (filestart)");
					log_message("INFO:            :  0x00 (filestart)");
					log_message("INFO:            :  0x00 (filestart)");
					log_message("INFO:            :  0x00 (filestart)");
					log_message("INFO:            :  %s", token_buffer[token]);
					log_message("INFO:            :  0x00");
				#endif
				sdf_write_unsigned_int(src_buffer+src_buffer_used, 0);  src_buffer_used+=4;
				sdf_write_unsigned_int(src_buffer+src_buffer_used, 0);  src_buffer_used+=4;
				sdf_write_unsigned_int(src_buffer+src_buffer_used, 0);  src_buffer_used+=4;
				sdf_write_unsigned_int(src_buffer+src_buffer_used, 0);  src_buffer_used+=4;
				sprintf(src_buffer+src_buffer_used, "%s", token_buffer[token]);
				src_buffer_used+=strlen(token_buffer[token])+1;
			}
			if(token_opcode[token] == OPCODE_IFFALSEJUMP)
			{
				// Jump function (if or while)...  2 byte address...  Filled in later...
				#ifdef VERBOSE_COMPILE
					log_message("INFO:            :  0x00");
					log_message("INFO:            :  0x00");
				#endif
				sprintf(src_buffer+src_buffer_used, "%c%c", 0, 0);
				src_buffer_used+=2;
			}
			if((token_opcode[token]&0xE0) == 0xA0 || token_opcode[token] == 227)
			{
				// 101xxxxx...  Integer variable with dot parameter...  One byte index follows
				// 227...  Integer numeric token....  1 byte data follows (signed)
				tempint = *((unsigned int*) &token_extension[token]);
				#ifdef VERBOSE_COMPILE
					log_message("INFO:            :  0x%02x", (tempint)&255);
				#endif
				*(src_buffer+src_buffer_used) = (tempint)&255;  src_buffer_used++;
			}
			if(token_opcode[token] == 228)
			{
				// Integer numeric token...  2 byte data follows (signed)...
				tempint = *((unsigned int*) &token_extension[token]);
				#ifdef VERBOSE_COMPILE
					log_message("INFO:            :  0x%02x", (tempint>>8)&255);
					log_message("INFO:            :  0x%02x", (tempint)&255);
				#endif
				*(src_buffer+src_buffer_used) = (tempint>>8)&255;  src_buffer_used++;
				*(src_buffer+src_buffer_used) = (tempint)&255;  src_buffer_used++;
			}
			if(token_opcode[token] == 229 || token_opcode[token] == 230)
			{
				// 229...  Integer numeric token...  4 byte data follows (signed)...
				// 230...  Float numeric token...  4 byte data follows (signed)...
				tempint = *((unsigned int*) &token_extension[token]);
				if(token_is_string[token])
				{
					// This is a string type of integer, so we need to remember where
					// it is, so functionize can change the numbers...  Write the opcode offset
					// into the string header area...
					#ifdef VERBOSE_COMPILE
						log_message("INFO:   Weird string fill-in-later thing.... %s", src_buffer+token_extension[token]);
					#endif
					sdf_write_unsigned_short((src_buffer+token_extension[token]-2), (src_buffer_used));
					tempint = 0;
				}
				#ifdef VERBOSE_COMPILE
					log_message("INFO:            :  0x%02x", (tempint>>24)&255);
					log_message("INFO:            :  0x%02x", (tempint>>16)&255);
					log_message("INFO:            :  0x%02x", (tempint>>8)&255);
					log_message("INFO:            :  0x%02x", (tempint)&255);
				#endif
				*(src_buffer+src_buffer_used) = (tempint>>24)&255;  src_buffer_used++;
				*(src_buffer+src_buffer_used) = (tempint>>16)&255;  src_buffer_used++;
				*(src_buffer+src_buffer_used) = (tempint>>8)&255;  src_buffer_used++;
				*(src_buffer+src_buffer_used) = (tempint)&255;  src_buffer_used++;
// !!!BAD!!!
// !!!BAD!!!
// !!!BAD!!!  Probably won't compile RUN's same on Macs...  Floats will be screwed up...  Maybe reverse bytes here...
// !!!BAD!!!
// !!!BAD!!!
			}


			// Now insert variable type conversions...
			if(token_change_type[token] == VAR_INT)
			{
				#ifdef VERBOSE_COMPILE
					log_message("INFO:         OPC:  0x%02x (to int)", OPCODE_TOINT);
				#endif
				src_buffer[src_buffer_used] = OPCODE_TOINT;  src_buffer_used++;
			}
			if(token_change_type[token] == VAR_FLOAT)
			{
				#ifdef VERBOSE_COMPILE
					log_message("INFO:         OPC:  0x%02x (to float)", OPCODE_TOFLOAT);
				#endif
				src_buffer[src_buffer_used] = OPCODE_TOFLOAT;  src_buffer_used++;
			}
		}
	}
	#ifdef VERBOSE_COMPILE
		log_message("INFO:   ");
	#endif
}

int src_find_string_entry(unsigned char* filedata, char* stringname)
{
	// <ZZ> This function searches a .RUN file's header for a given string, and returns
	//      the string's entry (offset in the header) if it finds a match.  If there weren't
	//      any matches, it returns 0.
	int number_of_functions;
	int number_of_strings;
	int i;
	unsigned short j;
	unsigned char* filedatastart;


	#ifdef VERBOSE_COMPILE
		log_message("INFO:   Looking for string %s...", stringname);
	#endif


	// For each function in the header...
	filedatastart = filedata;
	filedata+=MAX_FAST_FUNCTION<<1;  // Skip the fast function lookups...
	number_of_functions = sdf_read_unsigned_short(filedata);
	filedata+=2;
	number_of_strings = sdf_read_unsigned_short(filedata);
	filedata+=2;
	repeat(i, number_of_functions)
	{
		// Go to the next entry
		filedata+=2;                    // Skip the address
		filedata+=strlen(filedata)+1;   // Skip the name and 0
		filedata+=strlen(filedata)+1;   // Skip the return and any arguments and 0
	}
	repeat(i, number_of_strings)
	{
		// Only return strings that haven't been returned yet...  FFFF offset...
		j = sdf_read_unsigned_short(filedata);
		if(j == 65535)
		{
			// Check for a match...
			if(strcmp(stringname, filedata+2) == 0)
			{
				#ifdef VERBOSE_COMPILE
					log_message("INFO:   Found string %s at 0x%x", stringname, (filedata-filedatastart));
				#endif
				sdf_write_unsigned_short(filedata, 0);
				return (filedata-filedatastart);
			}
		}
		// Go to the next entry
		filedata+=2;                    // Skip the address
		filedata+=strlen(filedata)+1;   // Skip the name and 0
	}
	// Didn't find a match
	return 0;
}

unsigned char* src_mega_find_function(unsigned char* functionstring, unsigned char* filename, unsigned char request)
{
	// <ZZ> This function may only be called after a complete headerize has been performed.  It
	//      determines the header entry location of a function based on a "file.function" type
	//      string or just a "function" string, and then returns a bit of requested information
	//      about that function.  Filename is the name of the file in which the string was found,
	//      not necessarily the file the function is found in.  It returns NULL if no matching
	//      function could be found.
	unsigned char* functionname;
	unsigned char  must_fix_dot;
	unsigned char* data;
	unsigned int entry_offset;
	unsigned char* entry;
	unsigned char* index;

	// Split a two part string into two parts by figuring out where the dot is...
	must_fix_dot = FALSE;
	functionname = strpbrk(functionstring, ".");
	if(functionname != NULL)
	{
		// Found a dot, so we should have an explicit filename...
		filename = functionstring;
		*functionname = 0;    // Split into two strings...
		must_fix_dot = TRUE;  // ...But remember to put the dot back later...
		functionname++;
	}
	else
	{
		// No dot, so it's just a function name, no file name
		functionname = functionstring;
	}

	// Open the file that we think it's in...
	entry = NULL;
	// Found the file, so now look for the function
	data = sdf_find_filetype(filename, "RUN");
	if (data)
	{
		entry_offset = src_find_function_entry(data, functionname);
		if(entry_offset)
		{
			entry = data + entry_offset;
		}
	}

	// Didn't find the function on our first guess, so let's check GENERIC.RUN
	if (entry == NULL)
	{
		if (must_fix_dot == FALSE)
		{
			// Found the file, so now look for the function
			data = sdf_find_filetype("GENERIC", "RUN");
			if (data)
			{
				entry_offset = src_find_function_entry(data, functionname);
				if(entry_offset)
				{
					entry = data + entry_offset;
				}
			}
		}
	}

	// Put the dot back if we took it out...
	if (must_fix_dot)
	{
		*(functionname-1) = '.';
	}

	// Now see about filling the request...
	if(entry != NULL)
	{
		// And return whatever was requested...
		if(request == SRC_REQUEST_ENTRY)
		{
			return entry;
		}
		if(request == SRC_REQUEST_OFFSET)
		{
			return (data+sdf_read_unsigned_short(entry));
		}
		if(request == SRC_REQUEST_ARGUMENTS)
		{
			entry += 2;               // Skip the offset
			entry += strlen(entry)+1; // Skip the function name
			return entry;           // Return a pointer to the arguments
		}
		if(request == SRC_REQUEST_FILESTART)
		{
			return data;
		}
	}
	return NULL;
}

void src_set_priority(int start, int i, int end, signed char any_type)
{
	// <ZZ> This function is a helper for the RPN code.  It sets the given token to be the
	//      next most important token.


	if(i >= start && i <= end)
	{
		if(token_priority[i] == 0 && token_buffer[i][0] != ',')
		{
			if(token_is_operand[i] || any_type)
			{
				token_priority_list[token_order-1] = i;
				token_priority[i] = token_order;  token_order++;
			}
		}
	}
}

unsigned char* sdf_find_filetype(char *filename, char *filetype)
{
    // <ZZ> This function finds a matching entry, it returns a pointer
    //      to the data. If not, it returns NULL.

	char path[256];
	sprintf(path, "%s/%s.%s", working_dir, filename, filetype);

	if (read_file_to_buffer(path, &tempbuff) < 0)
		return NULL;

    return tempbuff.mem;
}

signed char src_define_setup(char *dirpath)
{
    // <ZZ> This function looks at "DEFINE.TXT" to see what global #define's we've got,
    //      including ID strings.  It returns TRUE if it worked okay, or FALSE if not.
    unsigned char indent;
	char path[256];
	sprintf(path, "%s/DEFINE.TXT", dirpath);

    obj_reset_property();
    next_token_may_be_negative = TRUE;
    src_num_define = 0;

    if (sdf_open(path))
    {
        log_message("INFO:   Setting up the global #defines...");
        while(sdf_read_line())
        {
            // Count the indentation (by 2's), and skip over any whitespace...
            indent = count_indentation(sdf_read_file);
            sdf_read_file+=indent;
            sdf_read_remaining-=indent;
            indent = indent>>1;

            // Check for a #define...
            if(sdf_read_file[0] == '#')
            {
                if(sdf_read_file[1] == 'd')
                {
                    if(sdf_read_file[2] == 'e')
                    {
                        if(sdf_read_file[3] == 'f')
                        {
                            if(sdf_read_file[4] == 'i')
                            {
                                if(sdf_read_file[5] == 'n')
                                {
                                    if(sdf_read_file[6] == 'e')
                                    {
                                        sdf_read_file+=7;
                                        sdf_read_remaining-=7;
                                        if(src_read_token(token_buffer[0]))
                                        {
                                            src_add_define(token_buffer[0], sdf_read_file, SRC_PERMANENT);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if(sdf_read_file[1] == 'p')
                {
                    if(sdf_read_file[2] == 'r')
                    {
                        if(sdf_read_file[3] == 'o')
                        {
                            if(sdf_read_file[4] == 'p')
                            {
                                if(sdf_read_file[5] == 'e')
                                {
                                    if(sdf_read_file[6] == 'r')
                                    {
                                        sdf_read_file+=7;
                                        sdf_read_remaining-=7;
                                        if(src_read_token(token_buffer[0]))   // tag
                                        {
                                            if(src_read_token(token_buffer[1]))  // type
                                            {
                                                if(src_read_token(token_buffer[2]))  // offset
                                                {
                                                    obj_add_property(token_buffer[0], token_buffer[1][0], token_buffer[2]);
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
		debug("INFO:   %d of %d properties used\n", obj_num_property, MAX_PROPERTY);
		debug("INFO:   %d of %d defines used\n", src_num_define, SRC_MAX_DEFINE);
        return TRUE;
    }
    debug("ERROR:  DEFINE.TXT could not be found in the database\n");
    return FALSE;
}

void obj_reset_property(void)
{
    // <ZZ> This function clears out our properties
    obj_num_property = 0;
}


void obj_add_property(char* tag, char type, char* offset)
{
    // <ZZ> This function registers a new property index...  The x in window.x for example.
    //      Tag is the name of the property, type is F or I or something, offset is the
    //      location of the data for this property relative to the start of the object's
    //      data (and is stored in ASCII text... "202" not the number 202).
    int i;

    if(obj_num_property < MAX_PROPERTY)
    {
        // Copy the name
        repeat(i, 8) { property_token[obj_num_property][i] = tag[i]; }
        property_token[obj_num_property][7] = 0;

        // Copy the type
        property_type[obj_num_property] = type;

        // Copy the offset
        sscanf(offset, "%d", &i);
        property_offset[obj_num_property] = i;

        #ifdef VERBOSE_COMPILE
            log_message("INFO:   Added property %s as number %d...  Type %c, Offset %d", property_token[obj_num_property], obj_num_property, property_type[obj_num_property], property_offset[obj_num_property]);
        #endif
        obj_num_property++;
    }
}

signed char sdf_open(char* filename)
{
    // <ZZ> This function opens a file in the database for reading line by line like a text
    //      document.  It returns TRUE if the file was found, or FALSE if not.
    sdf_read_line_number = 0;
    sdf_read_remaining = 0;
    sdf_read_first_line = FALSE;
    sdf_read_file = NULL;

    if (read_file_to_buffer(filename, &tempbuff) >= 0)
    {
        sdf_read_remaining = tempbuff.used;
        sdf_read_file = tempbuff.mem;
        return TRUE;
    }
    return FALSE;
}

void set_working_dir(char *path)
{
	working_dir = path;
}

void sfs_init(void)
{
	alloc_buffer(&tempbuff, 1024 * 1024);
}
