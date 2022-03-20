#include <errno.h>
#include <gattlib.h>
#include <stdlib.h>

#include "internal.h"
#include "logging.h"
#include "otslib/metadata.h"

#define OBJECT_NAME_UUID                "00002abe-0000-1000-8000-00805f9b34fb"
#define OBJECT_NAME_SIZE_MAX            120

#define OBJECT_TYPE_UUID                "00002abf-0000-1000-8000-00805f9b34fb"
#define OBJECT_TYPE_UUID16_SIZE         2
#define OBJECT_TYPE_UUID128_SIZE        16

#define OBJECT_SIZE_UUID                "00002ac0-0000-1000-8000-00805f9b34fb"
#define OBJECT_SIZE_SIZE                8

#define OBJECT_ID_UUID                  "00002ac3-0000-1000-8000-00805f9b34fb"
#define OBJECT_ID_SIZE                  6

#define OBJECT_PROP_UUID                "00002ac4-0000-1000-8000-00805f9b34fb"
#define OBJECT_PROP_SIZE                4

int otslib_get_name(void *adapter, char **name)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	void *buffer;
	size_t size;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	gattlib_string_to_uuid(OBJECT_NAME_UUID, strlen(OBJECT_NAME_UUID), &uuid);

	rc = gattlib_read_char_by_uuid(adpt->connection, &uuid, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read object name: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	if (size > OBJECT_NAME_SIZE_MAX) {
		rc = -EIO;
		LOG(LOG_ERR, "Received nbject name with size of %zu, maximum size is %u\n",
		    size, OBJECT_NAME_SIZE_MAX);
		goto free_memory;
	}

	if (name) {
		*name = calloc(1, size + 1);
		memcpy(*name, buffer, size);
		(*name)[size] = '\0';
		LOG(LOG_DEBUG, "Read nbject name: %s\n", *name);
	}

free_memory:
	free(buffer);

	return rc;
}

int otslib_set_name(void *adapter, const char *name)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	int rc = 0;
	size_t name_length = 0;

	if (adpt == NULL || name == NULL)
		return -EINVAL;

	name_length = strlen(name);
	if (name_length > OBJECT_NAME_SIZE_MAX) {
		LOG(LOG_ERR, "Cannot set object name with length of %zu, maximum length is %u\n",
		    name_length, OBJECT_NAME_SIZE_MAX);
		return -EINVAL;
	}

	gattlib_string_to_uuid(OBJECT_NAME_UUID, strlen(OBJECT_NAME_UUID), &uuid);

	rc = gattlib_write_char_by_uuid(adpt->connection, &uuid, name, strlen(name));
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not write object name: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	LOG(LOG_DEBUG, "Wrote object name: %s\n", name);

	return 0;
}

int otslib_type(void *adapter, uuid_t *uuid)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t u;
	void *buffer;
	size_t size;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	gattlib_string_to_uuid(OBJECT_TYPE_UUID, strlen(OBJECT_TYPE_UUID), &u);

	rc = gattlib_read_char_by_uuid(adpt->connection, &u, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read object type: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	if (uuid) {
		switch (size) {
		case OBJECT_TYPE_UUID16_SIZE:
			uuid->type = SDP_UUID16;
			uuid->value.uuid16 = bt_get_le16(buffer);
			LOG(LOG_DEBUG, "Read object type: 0x%04x\n", uuid->value.uuid16);
			break;
		case OBJECT_TYPE_UUID128_SIZE:
			LOG(LOG_ERR, "Unexpected object type size of %zu\n", size);
			rc = -ENOTSUP;
			break;
		default:
			LOG(LOG_ERR, "Unexpected object type size of %zu\n", size);
			rc = -EIO;
			break;
		}
	}

	free(buffer);

	return rc;
}

int otslib_size(void *adapter, size_t *current, size_t *allocated)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	void *buffer;
	size_t size;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	if (current == NULL && allocated == NULL)
		return -EINVAL;

	gattlib_string_to_uuid(OBJECT_SIZE_UUID, strlen(OBJECT_SIZE_UUID), &uuid);

	rc = gattlib_read_char_by_uuid(adpt->connection, &uuid, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read object size: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	if (size != OBJECT_SIZE_SIZE) {
		LOG(LOG_ERR, "Unexpected object size of %zu, expecting %u\n",
		    size, OBJECT_SIZE_SIZE);
		rc = -EIO;
		goto free_memory;
	}

	if (current) {
		*current = bt_get_le32(buffer);
		LOG(LOG_DEBUG, "Object current size: %zu\n", *current);
	}

	if (allocated) {
		*allocated = bt_get_le32((void *)((uintptr_t)buffer + 4));
		LOG(LOG_DEBUG, "Object allocated size: %zu\n", *allocated);
	}

free_memory:
	free(buffer);

	return rc;
}

int otslib_id(void *adapter, unsigned long long *id)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	void *buffer;
	size_t size;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	gattlib_string_to_uuid(OBJECT_ID_UUID, strlen(OBJECT_ID_UUID), &uuid);

	rc = gattlib_read_char_by_uuid(adpt->connection, &uuid, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read object ID: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	if (size != OBJECT_ID_SIZE) {
		rc = -EIO;
		LOG(LOG_ERR, "Received object ID with size %zu, expected size %u\n",
		    size, OBJECT_ID_SIZE);
		goto free_memory;
	}

	if (id) {
		/* ID is a 48-bit field, using a 64-bit temporary buffer makes life easier */
		uint8_t temp[sizeof(uint64_t)] = { 0 };
		memcpy(temp, buffer, OBJECT_ID_SIZE);
		*id = bt_get_le64(temp);
		LOG(LOG_DEBUG, "Object ID: 0x%012llx\n", *id);
	}

free_memory:
	free(buffer);

	return rc;
}

int otslib_properties(void *adapter, unsigned long *properties)
{
	struct otslib_adapter *adpt = (struct otslib_adapter *)adapter;
	uuid_t uuid;
	void *buffer;
	size_t size;
	int rc = 0;

	if (adpt == NULL)
		return -EINVAL;

	gattlib_string_to_uuid(OBJECT_PROP_UUID, strlen(OBJECT_PROP_UUID), &uuid);

	rc = gattlib_read_char_by_uuid(adpt->connection, &uuid, &buffer, &size);
	if (rc) {
		rc = map_error(rc);
		LOG(LOG_ERR, "Could not read object properties: %s (%d)\n",
		    strerror(rc), rc);
		return -rc;
	}

	if (size != OBJECT_PROP_SIZE) {
		rc = -EIO;
		LOG(LOG_ERR, "Received object property with size %zu, expected size %u",
		    size, OBJECT_PROP_SIZE);
		goto free_memory;
	}

	if (properties) {
		*properties = bt_get_le32(buffer);
		LOG(LOG_DEBUG, "Read object properties: 0x%08lx\n", *properties);
	}

free_memory:
	free(buffer);

	return rc;
}
