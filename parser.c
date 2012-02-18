/* PANDAseq -- Assemble paired FASTQ Illumina reads and strip the region between amplification primers.
     Copyright (C) 2011  Andre Masella

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "parser.h"

#define PARSE_CHUNK if (*input == '\0') return 0; for(;*input != '\0' && *input != ':' && *input != '#' && *input != '/' && *input != ' '; input++)
#define PARSE_INT do { value = 0; PARSE_CHUNK { if (*input >= '0' && *input <= '9') { value = 10*value + (*input - '0'); } else { return 0; } } } while(0)

int seqid_parse(seqidentifier * id, char *input)
{
	char *dest;
	int value;
	if (strchr(input, '#') != NULL) {
		/* Old CASAVA 1.4-1.6 format */
		id->run = 0;
		id->flowcell[0] = '\0';
		dest = id->instrument;
		PARSE_CHUNK {
			*dest++ = (*input);
		}
		input++;
		*dest = '\0';
		PARSE_INT;
		input++;
		id->lane = value;
		PARSE_INT;
		input++;
		id->tile = value;
		PARSE_INT;
		input++;
		id->x = value;
		PARSE_INT;
		input++;
		id->y = value;
		dest = id->tag;
		PARSE_CHUNK {
			if (dest >= &id->tag[8])
				return 0;
			*dest++ = (*input);
			*dest = '\0';
		}
		input++;
		PARSE_INT;
		input++;
		return value;
	} else {
		/* New CASAVA 1.7+ format */
		int mate;
		dest = id->instrument;
		PARSE_CHUNK {
			*dest++ = (*input);
		}
		input++;
		*dest = '\0';
		PARSE_INT;
		input++;
		id->run = value;
		dest = id->flowcell;
		PARSE_CHUNK {
			*dest++ = (*input);
		}
		input++;
		*dest = '\0';
		PARSE_INT;
		input++;
		id->lane = value;
		PARSE_INT;
		input++;
		id->tile = value;
		PARSE_INT;
		input++;
		id->x = value;
		PARSE_INT;
		input++;
		id->y = value;
		PARSE_INT;
		input++;
		mate = value;
		PARSE_CHUNK;
		input++;
		/* filtered */
		PARSE_INT;
		input++;
		/* control bits */
		dest = id->tag;
		PARSE_CHUNK {
			if (dest >= &id->tag[8])
				return 0;
			*dest++ = (*input);
			*dest = '\0';
		}
		return mate;
	}
}

void seqid_print(seqidentifier * id)
{
	printf(">%s:%d:%s:%d:%d:%d:%d:%s\n", id->instrument, id->run,
	       id->flowcell, id->lane, id->tile, id->x, id->y, id->tag);
}

int seqid_equal(seqidentifier * one, seqidentifier * two)
{
	return one->run == two->run && one->lane == two->lane
	    && one->tile == two->tile && one->x == two->x && one->y == two->y
	    && strcmp(one->instrument, two->instrument) == 0
	    && strcmp(one->flowcell, two->flowcell) == 0
	    && strcmp(one->tag, two->tag) == 0;
	;
}
