/***************************************************************************
 *   Copyright (C) 2009 by open-nandra                                     *
 *   				                                           *
 *   marek.belisko@open-nandra.com                                         *
 *                                                                         *
 *   For better understanding ihex format ihex_parser just parse data      *
 *   and print it in human readable way. This program was used to check    *
 *   what data are coming through USB to iqrf module when programming is.  *
 *   performed.								   *
 *									   *
 *    http://www.scienceprog.com/shelling-the-intel-8-bit-hex-file-format/ * 
 *    							   		   *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/* strings for record_type */
static const char *const record_type_desc[] = {
"Data record",
"End of file record",
"Extended Segment address record",
"Start segment address record",
"Extended Linear Address Record",
"Start Linear Address Record",	
};

/* print string */
void print_output(char *data)
{
	int byte_count = 0, record_type = 0, i = 0;
	char test_data[5];
	char * pEnd;
	sprintf(test_data, "%c%c", data[1], data[2]);
	
	byte_count = strtol(test_data, &pEnd, 16);
	
	printf("Byte count: %d\n", byte_count);


	printf("Addess: 0x%c%c%c%c\n",data[3], data[4], data[5], data[6]);
	sprintf(test_data, "%c%c", data[7], data[8]);
	record_type = atoi(test_data);
	printf("Record type: %s\n", record_type_desc[record_type]); 
	
	printf("Data:");
	for (i = 0; i < byte_count*2; i++)
		printf("%c",data[i+9]);

	printf("\n");
	
	printf("Checksum: %c%c\n", data[(byte_count*2) + 9], data[(byte_count*2) + 10]);
	printf("---------------------------------------------------------------");
	printf("\n");
}	

int main(int argc, char *argv[])
{
	FILE *in_file = 0;
	char in_data[100];
	
	if (argc == 1) {
		fprintf(stderr, "Using: ihex_parser <hex file>\n");
		goto exit;
	}


	in_file = fopen(argv[1], "r");
	
	if (in_file == NULL) {
		perror("open");
		goto exit;
	}

	while (fgets(in_data, 100,in_file)) {
		printf("line%s\n", in_data);
		print_output(in_data);
	}

exit:
	return 0;
	  
}