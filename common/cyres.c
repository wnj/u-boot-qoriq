/*
 * Copyright (c) Microsoft Corporation. All rights reserved.
 *
 * SPDX-License-Identifier:	BSD-3-Clause
 */
#include <common.h>
#include <version.h>
#include <cyres_cert_chain.h>
#include <cyres.h>

static int uboot_ret_from_cyres(cyres_result res)
{
	/*
	 * return values are compatible. U-Boot's only requirement is that
	 * success == 0.
	 */
	return (int)res;
}

int build_cyres_cert_chain(const struct build_cyres_cert_chain_args *args)
{
	cyres_result res;
	struct cyres_cert_blob *cert_blob = NULL;
	struct cyres_cert *cert = NULL;
	struct cyres_hw_identity identity;
	struct cyres_key_pair device_key_pair;
	struct cyres_key_pair next_image_key_pair;
	struct cyres_root_cert_args root_args;
	struct cyres_gen_alias_cert_args alias_args;

	res = read_and_hide_cyres_identity(&identity);
	if (res != 0) {
		debug("Cyres: failed to read and hide device identity\n");
		goto end;
	}

	res = cyres_init_cert_blob((void *)CONFIG_CYRES_CERT_CHAIN_ADDR,
				   CONFIG_CYRES_CERT_CHAIN_SIZE,
				   &cert_blob);

	if (res) {
		debug("Cyres: failed to initialize cert chain\n");
		goto end;
	}

	memset(&root_args, 0, sizeof(root_args));
	root_args.identity = (const uint8_t *)identity.data;
	root_args.identity_size = sizeof(identity.data);
	root_args.fwid = (const uint8_t *)U_BOOT_VERSION;
	root_args.fwid_size = sizeof(U_BOOT_VERSION);
	root_args.auth_key_pub = args->spl_auth_key_pub;
	root_args.auth_key_pub_size = args->spl_auth_key_pub_size;
	root_args.device_cert_subject = "SPL";
	root_args.root_path_len = CONFIG_CYRES_CERT_CHAIN_PATH_LENGTH;

	res = cyres_insert_root_and_device_certs(cert_blob, &root_args,
						 &device_key_pair);
	if (res) {
		debug("Failed to append root and device certs\n");
		goto end;
	}

	memset(&alias_args, 0, sizeof(alias_args));
	alias_args.issuer_key_pair = &device_key_pair;
	alias_args.seed_data = &device_key_pair.priv;
	alias_args.seed_data_size = sizeof(device_key_pair.priv);
	alias_args.subject_digest = args->next_image_digest;
	alias_args.subject_digest_size = args->next_image_digest_size;
	alias_args.auth_key_pub = args->next_image_auth_key_pub;
	alias_args.auth_key_pub_size = args->next_image_auth_key_pub_size;
	alias_args.subject_name = args->next_image_name;
	alias_args.issuer_name = "SPL";
	alias_args.path_len = 2;

	res = cyres_gen_alias_cert(&alias_args, &cert, &next_image_key_pair);
	if (res) {
		debug("Failed to generate cert for %s\n",
		      args->next_image_name);
		goto end;
	}

	res = cyres_insert_cert(cert_blob, cert);
	if (res) {
		debug("Failed to append cert (%s) to cert chain",
		      args->next_image_name);
		goto end;
	}

	res = cyres_make_key_blob_inplace((void *)CONFIG_CYRES_KEY_BLOB_ADDR,
					  CONFIG_CYRES_KEY_BLOB_SIZE,
					  &next_image_key_pair);

	if (res) {
		debug("Failed to store key.\n");
		goto end;
	}
end:

	if (cert)
		cyres_free_cert(cert);

	if (res != CYRES_SUCCESS) {
		memset((void *)CONFIG_CYRES_KEY_BLOB_ADDR, 0,
		       CONFIG_CYRES_KEY_BLOB_SIZE);

		memset((void *)CONFIG_CYRES_CERT_CHAIN_ADDR, 0,
		       CONFIG_CYRES_CERT_CHAIN_SIZE);
	}

	cyres_zero_mem(&identity, sizeof(identity));
	cyres_zero_mem(&device_key_pair, sizeof(device_key_pair));
	cyres_zero_mem(&next_image_key_pair, sizeof(next_image_key_pair));

	return uboot_ret_from_cyres(res);
}