/**
 * \file x509.h
 *
 *  Based on XySSL: Copyright (C) 2006-2008  Christophe Devine
 *
 *  Copyright (C) 2009  Paul Bakker <polarssl_maintainer at polarssl dot org>
 *
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of PolarSSL or XySSL nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef TROPICSSL_X509_H
#define TROPICSSL_X509_H

#define TROPICSSL_ERR_ASN1_OUT_OF_DATA                      -0x0014
#define TROPICSSL_ERR_ASN1_UNEXPECTED_TAG                   -0x0016
#define TROPICSSL_ERR_ASN1_INVALID_LENGTH                   -0x0018
#define TROPICSSL_ERR_ASN1_LENGTH_MISMATCH                  -0x001A
#define TROPICSSL_ERR_ASN1_INVALID_DATA                     -0x001C

#define TROPICSSL_ERR_X509_FEATURE_UNAVAILABLE              -0x0020
#define TROPICSSL_ERR_X509_CERT_INVALID_PEM                 -0x0040
#define TROPICSSL_ERR_X509_CERT_INVALID_FORMAT              -0x0060
#define TROPICSSL_ERR_X509_CERT_INVALID_VERSION             -0x0080
#define TROPICSSL_ERR_X509_CERT_INVALID_SERIAL              -0x00A0
#define TROPICSSL_ERR_X509_CERT_INVALID_ALG                 -0x00C0
#define TROPICSSL_ERR_X509_CERT_INVALID_NAME                -0x00E0
#define TROPICSSL_ERR_X509_CERT_INVALID_DATE                -0x0100
#define TROPICSSL_ERR_X509_CERT_INVALID_PUBKEY              -0x0120
#define TROPICSSL_ERR_X509_CERT_INVALID_SIGNATURE           -0x0140
#define TROPICSSL_ERR_X509_CERT_INVALID_EXTENSIONS          -0x0160
#define TROPICSSL_ERR_X509_CERT_UNKNOWN_VERSION             -0x0180
#define TROPICSSL_ERR_X509_CERT_UNKNOWN_SIG_ALG             -0x01A0
#define TROPICSSL_ERR_X509_CERT_UNKNOWN_PK_ALG              -0x01C0
#define TROPICSSL_ERR_X509_CERT_SIG_MISMATCH                -0x01E0
#define TROPICSSL_ERR_X509_CERT_VERIFY_FAILED               -0x0200
#define TROPICSSL_ERR_X509_KEY_INVALID_PEM                  -0x0220
#define TROPICSSL_ERR_X509_KEY_INVALID_VERSION              -0x0240
#define TROPICSSL_ERR_X509_KEY_INVALID_FORMAT               -0x0260
#define TROPICSSL_ERR_X509_KEY_INVALID_ENC_IV               -0x0280
#define TROPICSSL_ERR_X509_KEY_UNKNOWN_ENC_ALG              -0x02A0
#define TROPICSSL_ERR_X509_KEY_PASSWORD_REQUIRED            -0x02C0
#define TROPICSSL_ERR_X509_KEY_PASSWORD_MISMATCH            -0x02E0
#define TROPICSSL_ERR_X509_POINT_ERROR                      -0x0300
#define TROPICSSL_ERR_X509_VALUE_TO_LENGTH                  -0x0320

#define BADCERT_EXPIRED                 1
#define BADCERT_REVOKED                 2
#define BADCERT_CN_MISMATCH             4
#define BADCERT_NOT_TRUSTED             8

/*
 * DER constants
 */
#define ASN1_BOOLEAN                 0x01
#define ASN1_INTEGER                 0x02
#define ASN1_BIT_STRING              0x03
#define ASN1_OCTET_STRING            0x04
#define ASN1_NULL                    0x05
#define ASN1_OID                     0x06
#define ASN1_UTF8_STRING             0x0C
#define ASN1_SEQUENCE                0x10
#define ASN1_SET                     0x11
#define ASN1_PRINTABLE_STRING        0x13
#define ASN1_T61_STRING              0x14
#define ASN1_IA5_STRING              0x16
#define ASN1_UTC_TIME                0x17
#define ASN1_UNIVERSAL_STRING        0x1C
#define ASN1_BMP_STRING              0x1E
#define ASN1_PRIMITIVE               0x00
#define ASN1_CONSTRUCTED             0x20
#define ASN1_CONTEXT_SPECIFIC        0x80

/*
 * various object identifiers
 */
#define X520_COMMON_NAME                3
#define X520_COUNTRY                    6
#define X520_LOCALITY                   7
#define X520_STATE                      8
#define X520_ORGANIZATION              10
#define X520_ORG_UNIT                  11
#define PKCS9_EMAIL                     1

#define X509_OUTPUT_DER              0x01
#define X509_OUTPUT_PEM              0x02
#define PEM_LINE_LENGTH                72
#define X509_ISSUER                  0x01
#define X509_SUBJECT                 0x02

#define OID_X520                "\x55\x04"
#define OID_CN                  "\x55\x04\x03"
#define OID_PKCS1               "\x2A\x86\x48\x86\xF7\x0D\x01\x01"
#define OID_PKCS1_RSA           "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x01"
#define OID_PKCS1_RSA_SHA       "\x2A\x86\x48\x86\xF7\x0D\x01\x01\x05"
#define OID_PKCS9               "\x2A\x86\x48\x86\xF7\x0D\x01\x09"
#define OID_PKCS9_EMAIL         "\x2A\x86\x48\x86\xF7\x0D\x01\x09\x01"

/*
 * Structures for parsing X.509 certificates
 */
typedef struct _x509_buf {
  int tag;
  int len;
  const unsigned char *p;
} x509_buf;

typedef struct _x509_name {
  x509_buf oid;
  x509_buf val;
  struct _x509_name *next;
} x509_name;

typedef struct _x509_time {
  int year, mon, day;
  int hour, min, sec;
} x509_time;

typedef struct _x509_cert {
  x509_buf tbs;

  int version;
  x509_buf serial;
  x509_buf sig_oid1;

  x509_buf issuer_raw;
  x509_buf subject_raw;

  x509_name issuer;
  x509_name subject;

  x509_time valid_from;
  x509_time valid_to;

  x509_buf pk_oid;

  x509_buf issuer_id;
  x509_buf subject_id;
  x509_buf v3_ext;

  int ca_istrue;
  int max_pathlen;

  x509_buf sig_oid2;
  x509_buf sig;

} x509_cert;

#define SVN_X509_CERTINFO_KEY_ISSUER      "issuer"
#define SVN_X509_CERTINFO_KEY_VALID_FROM  "valid-from"
#define SVN_X509_CERTINFO_KEY_VALID_TO    "valid-to"

#ifdef __cplusplus
extern "C" {
#endif

  /* Parse x509 DER certificate data from BUF with length BUFLEN
   * and return certificate information in *CERT, allocated in
   * RESULT_POOL. The certinfo hash contains values of type
   * 'const char *' keyed by SVN_X509_CERTINFO_KEY_* macros. */
  int svn_x509_parse_cert(apr_hash_t **certinfo,
                          const char *buf,
                          int buflen,
                          apr_pool_t *result_pool,
                          apr_pool_t *scratch_pool);

/*
 * Store the name in printable form into buf; no more
 * than (end - buf) characters will be written
 */
int x509parse_dn_gets(char *buf, const char *end, const x509_name * dn);

#ifdef __cplusplus
}
#endif
#endif        /* x509.h */