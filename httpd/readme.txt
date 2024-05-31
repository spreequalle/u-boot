To modify Html files.

1. Run ./makefsdata

2. keep the modify html data

3. remove all const struct and define, use the const struct and define below
	In addition, keep the offset in const struct fsdata_file file_upgrade_fail_html[]

const struct fsdata_file file_404_html[] = {{NULL, data_404_html, data_404_html + 10, sizeof(data_404_html) - 10}};


const struct fsdata_file file_upgrade_fail_html[] = {{file_404_html, data_upgrade_fail_html, data_upgrade_fail_html + offset, sizeof(data_upgrade_fail_html) - offset}};

const struct fsdata_file file_upgrade_fail_index_html[] = {{file_upgrade_fail_html, data_index_html, data_index_html + 12, sizeof(data_index_html) - 12}};

const struct fsdata_file file_upgrade_fail_cgi[] = {{file_upgrade_fail_index_html, data_cgi_index, data_cgi_index + 11, sizeof(data_cgi_index) - 11}};

#define FS_ROOT_UPGRADE_FAIL file_upgrade_fail_cgi


const struct fsdata_file file_upgrade_success_html[] = {{file_404_html, data_upgrade_success_html, data_upgrade_success_html + 22, sizeof(data_upgrade_success_html) - 22}};

const struct fsdata_file file_upgrade_success_index_html[] = {{file_upgrade_success_html, data_index_html, data_index_html + 22, sizeof(data_index_html) - 22}};

const struct fsdata_file file_upgrade_success_cgi[] = {{file_upgrade_success_index_html, data_cgi_index, data_cgi_index + 11, sizeof(data_cgi_index) - 11}};

#define FS_ROOT_UPGRADE_SUCCESS file_upgrade_success_cgi


const struct fsdata_file file_restore_success_html[] = {{file_404_html, data_restore_success_html, data_restore_success_html + 12, sizeof(data_restore_success_html) - 12}};

const struct fsdata_file file_index_html[] = {{file_restore_success_html, data_index_html, data_index_html + 12, sizeof(data_index_html) - 12}};

const struct fsdata_file file_restore_success_cgi[] = {{file_index_html, data_cgi_index, data_cgi_index + 11, sizeof(data_cgi_index) - 11}};

#define FS_ROOT_RESTORE_SUCCESS file_restore_success_cgi


#define FS_NUMFILES 4 