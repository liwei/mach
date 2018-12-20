/*
 * newdes.c
 *
 * $Source: /afs/cs.cmu.edu/project/mach-2/rcs/server/as/newdes.c,v $
 *
 */

#ifndef	lint
static char     rcsid[] = "$Header: /afs/cs.cmu.edu/project/mach-2/rcs/server/as/newdes.c,v 1.1 88/08/05 00:38:34 mbj Exp $";
#endif not lint

/*
 * DES like encryption from:
 *	"Wide-Open Encryption Design offers flexible Implementations"
 *		by Robert Scott, Cryptologia January 1985.
 *
 */

/*
 * HISTORY:
 * 15-Jan-86  Robert Sansom (rds) at Carnegie Mellon University
 *	Started.
 *
 */

#include "key_defs.h"
#include "newdes.h"

static char farray[256] = {
    32, 137, 239, 188, 102, 125, 221, 72, 212, 68, 81, 37, 86, 237, 147, 149,
    70, 229, 17, 124, 115, 207, 33, 20, 122, 143, 25, 215, 51, 183, 138, 142,
    146, 211, 110, 173, 1, 228, 189, 14, 103, 78, 162, 36, 253, 167, 116, 255,
    158, 45, 185, 50, 98, 168, 250, 235, 54, 141, 195, 247, 240, 63, 148, 2,
    224, 169, 214, 180, 62, 22, 117, 108, 19, 172, 161, 159, 160, 47, 43, 171,
    194, 175, 178, 56, 196, 112, 23, 220, 89, 21, 164, 130, 157, 8, 85, 251,
    216, 44, 94, 179, 226, 38, 90, 119, 40, 202, 34, 206, 35, 69, 231, 246,
    29, 109, 74, 71, 176, 6, 60, 145, 65, 13, 77, 151, 12, 127, 95, 199,
    57, 101, 5, 232, 150, 210, 129, 24, 181, 10, 121, 187, 48, 193, 139, 252,
    219, 64, 88, 233, 96, 128, 80, 53, 191, 144, 218, 11, 106, 132, 155, 104,
    91, 136, 31, 42, 243, 66, 126, 135, 30, 26, 87, 186, 182, 154, 242, 123,
    82, 166, 208, 39, 152, 190, 113, 205, 114, 105, 225, 84, 73, 163, 99, 111,
    204, 61, 200, 217, 170, 15, 198, 28, 192, 254, 134, 234, 222, 7, 236, 248,
    201, 41, 177, 156, 92, 131, 67, 249, 245, 184, 203, 9, 241, 0, 27, 46,
    133, 174, 75, 18, 93, 209, 100, 120, 76, 213, 16, 83, 4, 107, 140, 52,
    58, 55, 3, 244, 97, 197, 238, 227, 118, 49, 79, 230, 223, 165, 153, 59
};



/*
 * newdesencrypt
 *	Encrypt text.
 *
 * Parameters:
 *	key	: the encryption key.
 *	text	: the data to be encrypted.
 *
 * Results:
 *	are left in text.
 *
 */
void newdesencrypt (key, text)
key_t		key;
block_t		text;
{
    register unsigned char b0, b1, b2, b3, b4, b5, b6, b7;
    b0 = text[0];
    b1 = text[1];
    b2 = text[2];
    b3 = text[3];
    b4 = text[4];
    b5 = text[5];
    b6 = text[6];
    b7 = text[7];

    b4 ^= farray[(b0 ^ key.key_bytes[0])];
    b5 ^= farray[(b1 ^ key.key_bytes[1])];
    b6 ^= farray[(b2 ^ key.key_bytes[2])];
    b7 ^= farray[(b3 ^ key.key_bytes[3])];
    b1 ^= farray[(b4 ^ key.key_bytes[4])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[5])];
    b0 ^= farray[(b7 ^ key.key_bytes[6])];
    b4 ^= farray[(b0 ^ key.key_bytes[7])];
    b5 ^= farray[(b1 ^ key.key_bytes[8])];
    b6 ^= farray[(b2 ^ key.key_bytes[9])];
    b7 ^= farray[(b3 ^ key.key_bytes[10])];
    b1 ^= farray[(b4 ^ key.key_bytes[11])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[12])];
    b0 ^= farray[(b7 ^ key.key_bytes[13])];
    b4 ^= farray[(b0 ^ key.key_bytes[14])];
    b5 ^= farray[(b1 ^ key.key_bytes[0])];
    b6 ^= farray[(b2 ^ key.key_bytes[1])];
    b7 ^= farray[(b3 ^ key.key_bytes[2])];
    b1 ^= farray[(b4 ^ key.key_bytes[3])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[4])];
    b0 ^= farray[(b7 ^ key.key_bytes[5])];
    b4 ^= farray[(b0 ^ key.key_bytes[6])];
    b5 ^= farray[(b1 ^ key.key_bytes[7])];
    b6 ^= farray[(b2 ^ key.key_bytes[8])];
    b7 ^= farray[(b3 ^ key.key_bytes[9])];
    b1 ^= farray[(b4 ^ key.key_bytes[10])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[11])];
    b0 ^= farray[(b7 ^ key.key_bytes[12])];
    b4 ^= farray[(b0 ^ key.key_bytes[13])];
    b5 ^= farray[(b1 ^ key.key_bytes[14])];
    b6 ^= farray[(b2 ^ key.key_bytes[0])];
    b7 ^= farray[(b3 ^ key.key_bytes[1])];
    b1 ^= farray[(b4 ^ key.key_bytes[2])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[3])];
    b0 ^= farray[(b7 ^ key.key_bytes[4])];
    b4 ^= farray[(b0 ^ key.key_bytes[5])];
    b5 ^= farray[(b1 ^ key.key_bytes[6])];
    b6 ^= farray[(b2 ^ key.key_bytes[7])];
    b7 ^= farray[(b3 ^ key.key_bytes[8])];
    b1 ^= farray[(b4 ^ key.key_bytes[9])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[10])];
    b0 ^= farray[(b7 ^ key.key_bytes[11])];
    b4 ^= farray[(b0 ^ key.key_bytes[12])];
    b5 ^= farray[(b1 ^ key.key_bytes[13])];
    b6 ^= farray[(b2 ^ key.key_bytes[14])];
    b7 ^= farray[(b3 ^ key.key_bytes[0])];
    b1 ^= farray[(b4 ^ key.key_bytes[1])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[2])];
    b0 ^= farray[(b7 ^ key.key_bytes[3])];
    b4 ^= farray[(b0 ^ key.key_bytes[4])];
    b5 ^= farray[(b1 ^ key.key_bytes[5])];
    b6 ^= farray[(b2 ^ key.key_bytes[6])];
    b7 ^= farray[(b3 ^ key.key_bytes[7])];
    b1 ^= farray[(b4 ^ key.key_bytes[8])];
    b2 ^= farray[(b4 ^ b5)];
    b3 ^= farray[(b6 ^ key.key_bytes[9])];
    b0 ^= farray[(b7 ^ key.key_bytes[10])];
    b4 ^= farray[(b0 ^ key.key_bytes[11])];
    b5 ^= farray[(b1 ^ key.key_bytes[12])];
    b6 ^= farray[(b2 ^ key.key_bytes[13])];
    b7 ^= farray[(b3 ^ key.key_bytes[14])];

    text[0] = b0;
    text[1] = b1;
    text[2] = b2;
    text[3] = b3;
    text[4] = b4;
    text[5] = b5;
    text[6] = b6;
    text[7] = b7;
}



/*
 * newdesdecrypt
 *	decrypt text.
 *
 * Parameters:
 *	key	: the decryption key.
 *	text	: the text to be decrypted.
 *
 * Results:
 *	left in text.
 *
 */
void newdesdecrypt (key, text)
key_t		key;
block_t		text;
{
    register unsigned char b0, b1, b2, b3, b4, b5, b6, b7;

    b0 = text[0];
    b1 = text[1];
    b2 = text[2];
    b3 = text[3];
    b4 = text[4];
    b5 = text[5];
    b6 = text[6];
    b7 = text[7];

    b4 ^=  farray[(b0 ^ key.key_bytes[11])];
    b5 ^=  farray[(b1 ^ key.key_bytes[12])];
    b6 ^=  farray[(b2 ^ key.key_bytes[13])];
    b7 ^=  farray[(b3 ^ key.key_bytes[14])];
    b1 ^=  farray[(b4 ^ key.key_bytes[8])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[9])];
    b0 ^=  farray[(b7 ^ key.key_bytes[10])];
    b4 ^=  farray[(b0 ^ key.key_bytes[4])];
    b5 ^=  farray[(b1 ^ key.key_bytes[5])];
    b6 ^=  farray[(b2 ^ key.key_bytes[6])];
    b7 ^=  farray[(b3 ^ key.key_bytes[7])];
    b1 ^=  farray[(b4 ^ key.key_bytes[1])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[2])];
    b0 ^=  farray[(b7 ^ key.key_bytes[3])];
    b4 ^=  farray[(b0 ^ key.key_bytes[12])];
    b5 ^=  farray[(b1 ^ key.key_bytes[13])];
    b6 ^=  farray[(b2 ^ key.key_bytes[14])];
    b7 ^=  farray[(b3 ^ key.key_bytes[0])];
    b1 ^=  farray[(b4 ^ key.key_bytes[9])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[10])];
    b0 ^=  farray[(b7 ^ key.key_bytes[11])];
    b4 ^=  farray[(b0 ^ key.key_bytes[5])];
    b5 ^=  farray[(b1 ^ key.key_bytes[6])];
    b6 ^=  farray[(b2 ^ key.key_bytes[7])];
    b7 ^=  farray[(b3 ^ key.key_bytes[8])];
    b1 ^=  farray[(b4 ^ key.key_bytes[2])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[3])];
    b0 ^=  farray[(b7 ^ key.key_bytes[4])];
    b4 ^=  farray[(b0 ^ key.key_bytes[13])];
    b5 ^=  farray[(b1 ^ key.key_bytes[14])];
    b6 ^=  farray[(b2 ^ key.key_bytes[0])];
    b7 ^=  farray[(b3 ^ key.key_bytes[1])];
    b1 ^=  farray[(b4 ^ key.key_bytes[10])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[11])];
    b0 ^=  farray[(b7 ^ key.key_bytes[12])];
    b4 ^=  farray[(b0 ^ key.key_bytes[6])];
    b5 ^=  farray[(b1 ^ key.key_bytes[7])];
    b6 ^=  farray[(b2 ^ key.key_bytes[8])];
    b7 ^=  farray[(b3 ^ key.key_bytes[9])];
    b1 ^=  farray[(b4 ^ key.key_bytes[3])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[4])];
    b0 ^=  farray[(b7 ^ key.key_bytes[5])];
    b4 ^=  farray[(b0 ^ key.key_bytes[14])];
    b5 ^=  farray[(b1 ^ key.key_bytes[0])];
    b6 ^=  farray[(b2 ^ key.key_bytes[1])];
    b7 ^=  farray[(b3 ^ key.key_bytes[2])];
    b1 ^=  farray[(b4 ^ key.key_bytes[11])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[12])];
    b0 ^=  farray[(b7 ^ key.key_bytes[13])];
    b4 ^=  farray[(b0 ^ key.key_bytes[7])];
    b5 ^=  farray[(b1 ^ key.key_bytes[8])];
    b6 ^=  farray[(b2 ^ key.key_bytes[9])];
    b7 ^=  farray[(b3 ^ key.key_bytes[10])];
    b1 ^=  farray[(b4 ^ key.key_bytes[4])];
    b2 ^=  farray[(b4 ^ b5)];
    b3 ^=  farray[(b6 ^ key.key_bytes[5])];
    b0 ^=  farray[(b7 ^ key.key_bytes[6])];
    b4 ^=  farray[(b0 ^ key.key_bytes[0])];
    b5 ^=  farray[(b1 ^ key.key_bytes[1])];
    b6 ^=  farray[(b2 ^ key.key_bytes[2])];
    b7 ^=  farray[(b3 ^ key.key_bytes[3])];

    text[0] = b0;
    text[1] = b1;
    text[2] = b2;
    text[3] = b3;
    text[4] = b4;
    text[5] = b5;
    text[6] = b6;
    text[7] = b7;

}
