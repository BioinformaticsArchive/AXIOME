/* Separate Illumina FASTQ reads by index tag and discard any degenerate sequences */
#include<bzlib.h>
#include<ctype.h>
#include<errno.h>
#if !defined(__APPLE__)
#include<error.h>
#endif
#include<fcntl.h>
#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<time.h>
#include<unistd.h>
#include<zlib.h>
#include<trie.h>
#include "kseq.h"

/* Function pointers for file I/O such that we can deal with compressed files. */
void *(*fileopen) (char *, char *) = (void *(*)(char *, char *))gzopen;
int (*fileread) (void *, void *, int) = (int (*)(void *, void *, int))gzread;
int (*fileclose) (void *) = (int (*)(void *))gzclose;

/* Compatibility function to make BZ2_bzRead look like gzread. */
int bzread(BZFILE * file, void *buf, int len)
{
	int bzerror = BZ_OK;
	int retval = BZ2_bzRead(&bzerror, file, buf, len);
	if (bzerror == BZ_OK || bzerror == BZ_STREAM_END) {
		return retval;
	} else {
		fprintf(stderr, "bzip error %d\n", bzerror);
		return -1;
	}
}

static unsigned long sdbm(str)
unsigned char *str;
{
	unsigned long hash = 0;
	int c;

	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}

KSEQ_INIT(void *, fileread)

int main(int argc, char **argv)
{
	int c;
	int bzip = 0;
	char *filename = NULL;
	void *file;
	kseq_t *seq;
	int len;
	int n = 0;
	Trie known_hashes = trie_create(NULL, 4);
	Trie files = trie_create(fclose, 4);

	/* Process command line arguments. */
	while ((c = getopt(argc, argv, "jf:")) != -1) {
		switch (c) {
		case 'j':
			fileopen = (void *(*)(char *, char *))BZ2_bzopen;
			fileread = (int (*)(void *, void *, int))bzread;
			fileclose = (int (*)(void *))BZ2_bzclose;
			bzip = 1;
			break;
		case 'f':
			filename = optarg;
			break;
		case '?':
			if (optopt == (int)'f') {
				fprintf(stderr,
					"Option -%c requires an argument.\n",
					optopt);
			} else if (isprint(optopt)) {
				fprintf(stderr,
					"Unknown option `-%c'.\n", optopt);
			} else {
				fprintf(stderr,
					"Unknown option character `\\x%x'.\n",
					(unsigned int)optopt);
			}
			return 1;
		default:
			abort();
		}
	}

	if (filename == NULL) {
		fprintf(stderr,
			"Usage: %s [-j] -f file.fastq\n\t-j\tInput files are bzipped.\n",
			argv[0]);
		return 1;
	}

	/* Open files and initialise FASTQ reader. */
	file = fileopen(filename, "r");
	if (file == NULL) {
		perror(filename);
		return 1;
	}
	seq = kseq_init(file);
	while ((len = kseq_read(seq)) >= 0) {
		unsigned long hash;
		int index;
		int ncount = 0;
		n++;
		for (index = 0; index < seq->seq.l; index++) {
			if (seq->seq.s[index] == 'N') {
				ncount++;
				break;
			}
		}
		if (ncount > 0) {
			fprintf(stderr, "SKIP %s\n", seq->name.s);
			continue;
		}

		hash = sdbm(seq->seq.s);
		if (trie_getf(known_hashes, &hash, sizeof(hash)) == NULL) {
			FILE *f;
			int indextag;
			trie_addf(known_hashes, &hash, sizeof(hash), &main,
				  true);

			for (index = 0; index < seq->name.l; index++) {
				if (seq->name.s[index] == '#') {
					break;
				}
			}
			if (index + 6 < seq->name.l)
				continue;
			seq->name.s[index + 6] = '\0';
			f = trie_get(files, seq->name.s + index);
			if (f == NULL) {
				char buffer[FILENAME_MAX];
				snprintf(buffer, FILENAME_MAX, "%s.%s",
					 filename, seq->name.s + index);
				f = fopen(buffer, "w");
				if (f == NULL) {
					perror(buffer);
					return 1;
				}
				trie_add(files, seq->name.s + index, f, true);
				fprintf(stderr, "FOPN %s\n", buffer);
			}
			fprintf(f, ">%s_%d\n%s\n", seq->name.s + index, n,
				seq->seq.s);
		} else {
			fprintf(stderr, "DUPL %s\n", seq->name.s);
		}
	}
	kseq_destroy(seq);
	trie_destroy(files);
	trie_destroy(known_hashes);
	if (fileclose(file) != Z_OK && bzip == 0) {
		perror(filename);
	}
	return 0;
}
