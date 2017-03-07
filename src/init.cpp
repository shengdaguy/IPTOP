#include "init.h"
#include "global.h"
#include <assert.h>
#include "util.h"
#include <fstream>

static int config_parse(const char *fname, Json::Value &conf) {
    fstream fs;
    fs.open(fname);
    if (!fs.is_open()) {
        fprintf(stderr, "%s not exist!\n", fname);
        return -1;
    }

    Json::Reader reader;
    int ok = reader.parse(fs, conf);
    fs.close();

    if (!ok) {
        fprintf(stderr, "%s parse failed!\n", fname);
        return -1;
    }

    return 0;
}

bool init_config() {
    
    int err = pthread_mutex_init(&net_info_lock, NULL);
    if (err != 0) return false;

    //TODO set the stdout buf
    setvbuf(stdout, 0, _IONBF, 0);

    int ret = config_parse("./iptop.json", g_conf);
    if (ret < 0) return false;

    g_report_url = g_conf["report"]["curl"].asString();
    g_report_interval = g_conf["report"]["interval"].asInt();

    ret = config_parse(g_conf["lbase_cfg"].asString().c_str(), g_lbase);
    if (ret < 0) return false;
    g_local_addr = g_lbase["addr"].asString();

    if (g_conf["daemon"].asInt()) {
        daemon(1, 0);
    }

    Json::FastWriter writer;
    printf("%s", writer.write(g_conf).c_str());
    printf("%s", writer.write(g_lbase).c_str());
    
    init_filter_ruler();

    return true;
}

void init_filter_ruler() {

    generic_filter_ruler = "ip and ( ";
    get_local_ip();
    int last = 1, size = local_ips.size();
    for (int i = 0; i < size; i++) {
        last = i == size - 1;
//        puts("-----------------");
 //       puts(local_ips[i].c_str());

        generic_filter_ruler += "host ";
        generic_filter_ruler += local_ips[i].c_str();
        generic_filter_ruler += " ";
        if (!last) 
            generic_filter_ruler += " or ";
    }

    generic_filter_ruler += ")";
    puts(generic_filter_ruler.c_str());
}

bool init_curl() {

    curl_global_init(CURL_GLOBAL_ALL);
    /* get a curl handle */
    g_curl_rp = curl_easy_init();
    assert(g_curl_rp);

    curl_easy_setopt(g_curl_rp, CURLOPT_URL, g_report_url.c_str());

    g_list = NULL;
    g_list = curl_slist_append(g_list, "Content-Type: application/json");

    curl_easy_setopt(g_curl_rp, CURLOPT_HTTPHEADER, g_list);

    return true;
}
