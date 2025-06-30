#define FNV_PRIME_32 0x93
#define FNV_OFFSET_BASIS_32 2166136261

#define HM_MIN_SIZE 32

/// @brief Simple 32bit hash function
/// @param input string
/// @return 32bit hash
int fnv1a_32(char *input);

struct HashMap
{
    int size;
    int count;
    char **keys;
    void **values;
};

/// @brief Creates a new hashmap
struct HashMap *HM_Init();

/// @brief No need to call manually. Checks if now is a good time to expand the underlying arrays.
/// @param hm the hashmap to check
/// @return 1 if the hashmap is too full (>75% occupied), 0 otherwise
int HM_ShouldExpand(struct HashMap *hm);

/// @brief Grows the hashmap twice in size.
/// @param hm the hashmap to grow.
void HM_Expand(struct HashMap *hm);

/// @brief No need to call manually. Checks if now is a good time to shrink the underlying arrays.
/// @param hm the hashmap to check
/// @return 1 if the hashmap is too empty (<50% occupied), 0 otherwise
int HM_ShouldShrink(struct HashMap *hm);

/// @brief Shrinks the hashmap to half its size.
/// @param hm the hashmap to shrink
void HM_Shrink(struct HashMap *hm);

/// @brief Inserts the entry into the hashmap.
/// @note Crashes upon encountering a collision. Use `HM_Update` if you want to overwrite the old value instead.
/// @param hm the hashmap to modify
/// @param key
/// @param value pointer to the value
void HM_Insert(struct HashMap *hm, char *key, void *value);

/// @brief Removes the entry from the hashmap. Doesn't check if it existed before.
/// @param hm the hashmap to modify
/// @param key
void HM_Remove(struct HashMap *hm, char *key);

/// @brief Retrieves the corresponding value from the hashmap.
/// @param hm the hashmap to search in
/// @param key
/// @return The pointer to the value. If the value wasn't found, returns `NULL`.
void *HM_Get(struct HashMap *hm, char *key);

/// @brief Overwrites or inserts the entry into the corresponding hashmap.
/// @param hm the hashmap to insert into
/// @param key
/// @param value
void HM_Update(struct HashMap *hm, char *key, void *value);

/// @brief Cleans up all used resources.
/// @param hm the hashmap to whipe from existence
void HM_Deinit(struct HashMap *hm);