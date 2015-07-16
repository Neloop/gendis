/*
 * Given to us on 5-th lecture from our professor
 */

#include "common.h"

#include <netinet/in.h>
#include <inttypes.h>
#include <string.h>
#include <openssl/sha.h>
#include <unistd.h>

#include <stdio.h>

#define	USIZE	21

typedef union {
	unsigned char s[USIZE];
	struct {
		uid_t uid1;
		uid_t uid2;
		uint32_t nonce1;
		uint32_t nonce2;
		uint32_t key;
		unsigned char hand;
	} f;
} mic;

static void dump_mem(unsigned char *buf, size_t len) {
	fprintf(stderr, "dump_mem %p:", buf);
	while (len--)
		fprintf(stderr, "%02x ", (unsigned int)*buf++);
	fprintf(stderr, "\n");
}

unsigned char *
sign(uid_t uid1, uid_t uid2, uint32_t nonce1, uint32_t nonce2, uint32_t key,
		unsigned char hand) {

	mic m;

	m.f.uid1 = htons(uid1);
	m.f.uid2 = htons(uid2);
	m.f.nonce1 = htonl(nonce1);
	m.f.nonce2 = htonl(nonce2);
	m.f.key = htonl(key);
	m.f.hand = hand;

	return (SHA256(m.s, USIZE, NULL));
}

unsigned char
verify(uid_t uid1, uid_t uid2, uint32_t nonce1, uint32_t nonce2, uint32_t key,
		unsigned char *hash) {

	unsigned char c = 3;
	unsigned char *aux;

	while (c) {
		aux = sign(uid1, uid2, nonce1, nonce2, key, c);
		if (!memcmp(hash, aux, SHA256_DIGEST_LENGTH))
			break;
		c--;
	}

	return (c);
}

int
evaluate(unsigned char hand1, unsigned char hand2) {
	static int ev[3][3] = { {0, 1, 2}, {2, 0, 1}, {1, 2, 0}};

	hand1--;
	hand2--;
	if ((hand1 > 2) || (hand2 > 2))
		return (-1);

	return (ev[hand1][hand2]);
}

char *
hand2str(unsigned char hand) {
	static char *arr[3] = {"rock", "scissors", "paper"};

	if (--hand > 2)
		return (NULL);

	return (arr[hand]);

}
