#ifndef _H_OFTABLE_
#define _H_OFTABLE_

void
sgx_table_dpif_init(uint8_t bridge_id, int n_tables);
static void
oftable_init(struct oftable * table);
void
sgx_table_cls_init(uint8_t bridge_id);
void
sgx_oftable_destroy(uint8_t bridge_id, uint8_t table_id);

struct oftable *
next_matching_table(uint8_t bridge_id, int ofproto_n_tables, const struct oftable * table, uint8_t table_id);
struct oftable *
first_matching_table(uint8_t bridge_id, int ofproto_n_tables, uint8_t table_id);
struct oftable *
next_visible_table(uint8_t bridge_id, int ofproto_n_tables, uint8_t table_id);


#endif
