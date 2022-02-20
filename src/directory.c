#include <errno.h>
#include <gattlib.h>
#include <stdlib.h>

#include "internal.h"
#include "logging.h"
#include "otslib/action.h"
#include "otslib/directory.h"
#include "otslib/metadata.h"
#include "otslib/list.h"

int otslib_directory_read(void *adapter, void **buffer)
{
	if (adapter == NULL)
		return -EINVAL;

	if (buffer == NULL)
		return -EINVAL;

	int rc = otslib_goto(adapter, OTSLIB_OBJECT_ID_DIRECTORY_LISTING);
	if (rc) {
		LOG(LOG_ERR, "Could not select directory listing object: %d\n", rc);
		return rc;
	}

	size_t current_size;
	rc = otslib_size(adapter, &current_size, NULL);
	if (rc) {
		LOG(LOG_ERR, "Could not get directory listing object size: %d\n", rc);
		return rc;
	}

	rc = otslib_read(adapter, 0, current_size, buffer);
	if (rc < 0) {
		LOG(LOG_ERR, "Could not read directory listing object: %d\n", rc);
		return rc;
	}
	if (rc != current_size) {
		LOG(LOG_ERR, "Only read %zu from the expected %zu\n", rc, current_size);
		return rc;
	}

	return rc;
}

int otslib_directory_for_each(void *buffer, size_t length, void *user_data, int (*callback)(const struct otslib_object_metadata *metadata, void *user_data))
{
	if (buffer == NULL)
		return -EINVAL;

	if (callback == NULL)
		return -EINVAL;

	off_t entry_offset = 0;

	while (entry_offset < length) {
		uint8_t *p = (uint8_t *)buffer + entry_offset;
		off_t offset = 0;

		size_t entry_length = bt_get_le16(p + offset);
		offset += sizeof(uint16_t);

		struct otslib_object_metadata metadata = { 0 };

		metadata.id = bt_get_le16(p + offset);
		offset += sizeof(uint16_t);
		metadata.id |= bt_get_le32(p + offset) << sizeof(uint16_t);
		offset += sizeof(uint32_t);

		uint8_t name_length = *(p + offset);
		offset += sizeof(uint8_t);

		char *name = malloc(name_length + 1);
		memcpy(name, p + offset, name_length);
		name[name_length] = '\0';
		metadata.name = name;
		offset += name_length;

		uint8_t flags = *(p + offset);
		offset += sizeof(uint8_t);

		if (flags & OTSLIB_DIRECTORY_FLAG_OBJ_TYPE_UUID128) {
			metadata.type.type = SDP_UUID128;
			LOG(LOG_ERR, "Unexpected object type of UUI128: %u\n", SDP_UUID128);
			return -ENOTSUP;
		} else {
			metadata.type.type = SDP_UUID16;
			metadata.type.value.uuid16 = bt_get_le16(p + offset);
			offset += sizeof(uint16_t);
		}

		if (flags & OTSLIB_DIRECTORY_FLAG_CURRENT_SIZE) {
			metadata.current_size = bt_get_le32(p + offset);
			offset += sizeof(uint32_t);
		} else {
			metadata.current_size = -1;
		}

		if (flags & OTSLIB_DIRECTORY_FLAG_ALLOCATED_SIZE) {
			metadata.allocated_size = bt_get_le32(p + offset);
			offset += sizeof(uint32_t);
		} else {
			metadata.allocated_size = -1;
		}

		if (flags & OTSLIB_DIRECTORY_FLAG_FIRST_CREATED)
			offset += 7;

		if (flags & OTSLIB_DIRECTORY_FLAG_LAST_MODIFIED)
			offset += 7;

		if (flags & OTSLIB_DIRECTORY_FLAG_PROPERTIES) {
			metadata.properties = bt_get_le32(p + offset);
			offset += sizeof(uint32_t);
		}

		int rc = callback(&metadata, user_data);
		free(name);
		metadata.name = NULL;

		if (rc)
			return rc;

		entry_offset += entry_length;
	}

	return 0;
}
