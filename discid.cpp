#include "foo_musicbrainz.h"

char *get_discid(int tracks_count, unsigned int *tracks_lengths, int pregap) {
	unsigned int tracks[100];
	SHA1Context sha;
	SHA1Reset(&sha);
	char tmp[9];
	unsigned char digest[20];
	unsigned long discid_length;

	tracks[1] = pregap;
	for (int i = 2; i < tracks_count+1; i++) {
		tracks[i] = tracks[i-1] + tracks_lengths[i-2];
	}
	tracks[0] = tracks[tracks_count] + tracks_lengths[tracks_count-1];
	for (int i = tracks_count+1; i < 100; i++) {
		tracks[i] = 0;
	}

	sprintf(tmp, "%02X", 1);
	SHA1Input(&sha, (unsigned char *)tmp, 2);

	sprintf(tmp, "%02X", tracks_count);
	SHA1Input(&sha, (unsigned char *)tmp, 2);
	
	for (int i = 0; i < 100; i++) {
		sprintf(tmp, "%08X", tracks[i]);
		SHA1Input(&sha, (unsigned char *)tmp, 8);

	}

	SHA1Result(&sha, digest);
	char *discid = (char *)rfc822_binary(digest, 20, &discid_length);
	discid[discid_length] = '\0';

	return discid;
}

/*
 * Program:	RFC-822 routines (originally from SMTP)
 */

/* NOTE: This is not true RFC822 anymore. The use of the characters
   '/', '+', and '=' is no bueno when the ID will be used as part of a URL.
   '_', '.', and '-' have been used instead
*/

/* Convert binary contents to BASE64
 * Accepts: source
 *	    length of source
 *	    pointer to return destination length
 * Returns: destination as BASE64
 */

unsigned char *rfc822_binary (void *src,unsigned long srcl,unsigned long *len)
{
  unsigned char *ret,*d;
  unsigned char *s = (unsigned char *) src;
  char *v = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789._";
  unsigned long i = ((srcl + 2) / 3) * 4;
  *len = i += 2 * ((i / 60) + 1);
  d = ret = (unsigned char *) malloc ((size_t) ++i);
  for (i = 0; srcl; s += 3) {	/* process tuplets */
    *d++ = v[s[0] >> 2];	/* byte 1: high 6 bits (1) */
				/* byte 2: low 2 bits (1), high 4 bits (2) */
    *d++ = v[((s[0] << 4) + (--srcl ? (s[1] >> 4) : 0)) & 0x3f];
				/* byte 3: low 4 bits (2), high 2 bits (3) */
    *d++ = srcl ? v[((s[1] << 2) + (--srcl ? (s[2] >> 6) : 0)) & 0x3f] : '-';
				/* byte 4: low 6 bits (3) */
    *d++ = srcl ? v[s[2] & 0x3f] : '-';
    if (srcl) srcl--;		/* count third character if processed */
    if ((++i) == 15) {		/* output 60 characters? */
      i = 0;			/* restart line break count, insert CRLF */
      *d++ = '\015'; *d++ = '\012';
    }
  }
  *d = '\0';			/* tie off string */

  return ret;			/* return the resulting string */
}
