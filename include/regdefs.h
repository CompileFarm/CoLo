/*
 * (C) P.Horton 2004,2005,2006
 *
 * $Id: regdefs.h 186 2006-01-17 23:03:58Z pdh $
 *
 * This code is covered by the GNU General Public License. For details see the file "COPYING".
 */

#define zero			$0
#define at			$1
#define v0			$2
#define v1			$3
#define a0			$4
#define a1			$5
#define a2			$6
#define a3			$7
#define t0			$8
#define t1			$9
#define t2			$10
#define t3			$11
#define t4			$12
#define t5			$13
#define t6			$14
#define t7			$15
#define s0			$16
#define s1			$17
#define s2			$18
#define s3			$19
#define s4			$20
#define s5			$21
#define s6			$22
#define s7			$23
#define t8			$24
#define t9			$25
#define k0			$26
#define k1			$27
#define gp			$28
#define sp			$29
#define fp			$30
#define ra			$31

#define CP0_COMPARE		$11
#define CP0_STATUS		$12
# define CP0_STATUS_IM(n)	(1 << (8 + (n)))
#define CP0_CAUSE		$13
# define CP0_CAUSE_IP(n)	(1 << (8 + (n)))

/* vi:set ts=8 sw=8 ai: */
