/*
 * Given to us on 5-th lecture from our professor
 */

#ifndef	_COMMON_H_
#define	_COMMON_H_

#include <sys/types.h>
#include <stdint.h>

/*
 * Computes SHA256(uid1||uid2||nonce1||nonce2||key||hand) (||...concat)
 * Integers are represented as big endian.
 * User is not supposed to free the result.
 */
unsigned char *
sign(uid_t uid1, uid_t uid2, uint32_t nonce1, uint32_t nonce2, uint32_t key,
		unsigned char hand);
/*
 * Verifies, that hash is a valid signature for the given values and some hand.
 * Returns the hand (1, 2 or 3}, or 0 on error.
 */
unsigned char
verify(uid_t uid1, uid_t uid2, uint32_t nonce1, uint32_t nonce2, uint32_t key,
		unsigned char *hash);

/*
 * Given two hands, evaluates rock-paper-scissors game.
 * Hand: 1...rock, 2...scissors, 3...paper
 * Returns: 
 * 	 0 ... draw
 * 	 1 ... hand1 wins
 * 	 2 ... hand2 wins
 * 	-1 ... error
 */
int
evaluate(unsigned char hand1, unsigned char hand2);

/*
 * Returns string representation of hand.
 */
char *
hand2str(unsigned char hand);

#endif /* _COMMON_H_ */
