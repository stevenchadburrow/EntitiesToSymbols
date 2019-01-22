// EnitiesToSymbolsProgramV2.cpp
// Takes .kml file with actual Entities, and converts it to a .kml with only Symbols
// Programming performed by Steven Chad Burrow, Alpha Omega Solutions, for the DCGS-A Embedded Trainer Program.

#include <stdio.h> // standard libraries
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // used to get directory/folder information

// Convert() function does the actual conversion of the KML file
int Convert(const char *input_filename, const char *output_filename, const char *overlay_name)
{
	printf("Converting...  input_filename: %s, output_filename: %s, overlay_name: %s\n", 
		input_filename, output_filename, overlay_name);

	FILE *input = NULL, *output = NULL;

	input = fopen(input_filename, "rt"); // open input file
	if (!input) return 0;
	
	output = fopen(output_filename, "wt"); // open output file (always overwritten)
	if (!output) return 0;

	int bytes = 1; // number of bytes read in
	char buffer = 0; // the character read in

	char current_string[256]; // the last 256 characters read in
	char test_string[256]; // a specific amount of characters from the end, used to search and then replace
	char type_string[256]; // when dealing with entity names, will only convert specific types

	int type_string_length = 0; // length of type_string[]

	int folder_flag = 0; // points code to look for the next <name> tag after the <Folder> tag

	for (int i=0; i<256; i++)
	{
		current_string[i] = 0;
	}

	while (bytes > 0) // read to end of file
	{
		bytes = fscanf(input, "%c", &buffer); // read in each character

		if (bytes > 0)
		{
			for (int i=0; i<255; i++)
			{
				current_string[i] = current_string[i+1]; // shift current_string[] by one character
			}

			current_string[255] = buffer; // put last character into current_string[]
			
			fprintf(output, "%c", buffer); // output to file

			// Test for ViperType...

			for (int i=0; i<256; i++)
			{
				test_string[i] = 0;
			}

			memcpy(test_string, current_string + (256-29), 29); // put only 29 characters into test_string[]

			if (strcmp(test_string, "<SimpleData name=\"ViperType\">") == 0) // compare and see if this is an Entity
			{
				for (int i=0; i<256; i++)
				{
					type_string[i] = 0;
				}

				type_string_length = 0;

				buffer = 0;

				while (buffer != '<') // read in the full type of entity
				{
					fscanf(input, "%c", &buffer); // read in one character at a time

					if (buffer != '<')
					{
						type_string[type_string_length] = buffer; // store the type of entity in type_string[]

						type_string_length++;
					}
				}

				if (strcmp(type_string, "Unit") == 0 ||
					strcmp(type_string, "Equipment") == 0 ||
					strcmp(type_string, "Vehicle") == 0 ||
					strcmp(type_string, "Organization") == 0 ||
					strcmp(type_string, "Person") == 0) // only convert these types of entity types
				{
					fprintf(output, "%s%c", "MapPointIcon", buffer); // convert them into MapPointIcon's
				}
				else
				{
					fprintf(output, "%s%c", type_string, buffer); // otherwise, ignore them
				}
			}

			// Test for Placemark...

			for (int i=0; i<256; i++)
			{
				test_string[i] = 0;
			}

			memcpy(test_string, current_string + (256-10), 10); // put only 10 characters into test_string[]

			if (strcmp(test_string, "<Placemark") == 0) // compare and see if this is a Placemark
			{
				buffer = 0;

				while (buffer != '>')
				{
					fscanf(input, "%c", &buffer); // read in, but do not retain or output placemark information
				}

				fprintf(output, "%c", buffer); // always is a > symbol
			}

			// Test for Folder...

			if (folder_flag == 0) // when program as not yet seen the <Folder> tag
			{
				for (int i=0; i<256; i++)
				{
					test_string[i] = 0;
				}
	
				memcpy(test_string, current_string + (256-7), 7); // put only 7 characters into test_string[]
	
				if (strcmp(test_string, "<Folder") == 0) // compare and see if this is a Folder
				{
					folder_flag = 1; // get ready for next <name> tag
				}
			}
			else if (folder_flag == 1) // the Folder has already been seen, the next <name> tag needs to be changed
			{
				for (int i=0; i<256; i++)
				{
					test_string[i] = 0;
				}
	
				memcpy(test_string, current_string + (256-6), 6); // put only 6 characters into test_string[]

				if (strcmp(test_string, "<name>") == 0) // compare and see if this is a <name> tag
				{
					buffer = 0;
	
					while (buffer != '<')
					{
						fscanf(input, "%c", &buffer); // read in, but do not retain or output <name> information
					}
	
					fprintf(output, "%s%c", overlay_name, buffer); // instead, replace it with the overlay_name

					folder_flag = 0;
				}
			}
		}
	}

	fclose(input); // close files
	fclose(output);

	printf("Converting Complete!\n\n");

	return 1;
};

// main() function
int main(const int argc, const char **argv)
{
	printf("EntitiesToSymbolsProgramV2.exe\n");
	printf("Programming performed by Steven Chad Burrow, Alpha Omega Solutions, for the DCGS-A Embedded Trainer Program.\n");
	printf("Either have no parameters, or have 3 parameters: input_file output_file overlay_name\n\n");

	// Find local directory name... 

	char local_directory_name[2048];
	int slash_count = 0, slash_total = 0;

	for (int i=0; i<2048; i++)
	{
		local_directory_name[i] = 0;
	}

	strcpy(local_directory_name, argv[0]); // get the directory name from the program's name/location itself, but then we will need to remove the name of the program

	slash_count = 0;

	for (int i=0; i<2048; i++)
	{
		if (local_directory_name[i] == '/' || // for Linux
			local_directory_name[i] == '\\') // for Windows
		{
			slash_count++; // count how many slashes in the program's name/location
		}
	}

	slash_total = slash_count;
	slash_count = 0;

	for (int i=0; i<2048; i++)
	{
		if (slash_count == slash_total) // when we see the last slash,
		{
			local_directory_name[i] = 0; // clear out anything besides local directory name
		}

		if (local_directory_name[i] == '/' || // for Linux
			local_directory_name[i] == '\\') // for Windows
		{
			slash_count++; // just re-counting the slashes in the program's name/location, comes AFTER in the code so the last slash is not removed
		}
	}
	
	// If there were no parameters, search for particular .kml files in the folder that contains the program
	if (argc == 1)
	{
		DIR *dir;
		dirent *ent;

		char ext_string[1024], file_string[1024], custom_string[1024];

		char extra_string[1024];

		bool check_custom_flag = false;

		char full_length_input_filename[1024], full_length_output_filename[1024];

		for (int i=0; i<1024; i++)
		{
			full_length_input_filename[i] = 0;
			full_length_output_filename[i] = 0;
		}
	
		dir = opendir(local_directory_name); // tell the terminal to use the program's local directory for all work
		
		while (ent = readdir(dir)) // loops, giving one by one the results of a 'dir' call in terminal
		{
			// Test for [ .kml or .KML ] and not ****CustomSymbolsOverlay.kml...

			for (int i=0; i<1024; i++)
			{
				ext_string[i] = 0;
				file_string[i] = 0;
				custom_string[i] = 0;

				extra_string[i] = 0;
			}
			
			// 'dir' will return . and .. which are not to be used (memory issues)
			if ((unsigned)strlen(ent->d_name) > 4) // so only use files that are larger than ".kml"
			{
				memcpy(ext_string, ent->d_name + ((unsigned)strlen(ent->d_name)-4), 4); // make sure it's a .kml file
				memcpy(file_string, ent->d_name, (unsigned)strlen(ent->d_name)-4); // get the name of the file itself without the .kml at the end

				check_custom_flag = false;

				// a shorter filename cannot be a ****CustomSymbolsOverlay.kml file
				if ((unsigned)strlen(ent->d_name) >= 24) // so if there is a larger file, check to see if it was already output by the program
				{
					check_custom_flag = true;
				
					// get the last 24 characters to compare and see if it's already been created by the program
					memcpy(custom_string, ent->d_name + ((unsigned)strlen(ent->d_name)-24), 24); // if we didn't check the size before, there could be a memory issue
				}

				// it must be a .kml file, and not be a file that has already been created by the program
				if ((strcmp(ext_string, ".kml") == 0 || strcmp(ext_string, ".KML") == 0) &&
					(check_custom_flag == false || (check_custom_flag == true && !(strcmp(custom_string, "CustomSymbolsOverlay.kml") == 0))))
				{
					sprintf(extra_string, "%sCustomSymbolsOverlay.kml", file_string); // make it's new full name

					sprintf(full_length_input_filename, "%s%s", local_directory_name, ent->d_name); // put the local directory in the input file
					sprintf(full_length_output_filename, "%s%s", local_directory_name, extra_string); // put the local directory in the output file

					Convert(full_length_input_filename, full_length_output_filename, file_string); // use the Convert() function now
				}
			}
		}
		
		closedir(dir); // close the 'dir' call
	}
	else if (argc == 4)
	{
		Convert(argv[1], argv[2], argv[3]); // a simple but direct approach, might need full directory name attached?
	}
	else
	{
		printf("Parameter Error\n"); // if not given correct parameters, show error
	}

	return 0;
}


