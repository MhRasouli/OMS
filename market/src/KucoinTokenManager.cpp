#include "KucoinTokenManager.h"
#include <nlohmann/json.hpp>
#include <curl/curl.h>
#include <iostream>
#include "Logger.h"
using json = nlohmann::json;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

KucoinTokenManager::KucoinTokenManager(const std::string &api_url) : m_api_url(api_url)
{
    renew_token();
}

KucoinTokenManager::~KucoinTokenManager()
{
    stop_auto_renew();
}

std::string KucoinTokenManager::get_ws_url()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_endpoint + "?token=" + m_token;
}

void KucoinTokenManager::start_auto_renew()
{
    if (m_renew_thread.joinable())
    {
        return; // Already running
    }

    m_running = true;
    m_renew_thread = std::thread([this]()
                                 {
        int sleep_counter=0;
        while (m_running) {
            std::this_thread::sleep_for(std::chrono::seconds(1)); // Renew before expiry
            if (++sleep_counter==82800) {
                sleep_counter=0;
                renew_token();
            }
        } });
}

void KucoinTokenManager::stop_auto_renew()
{
    m_running = false;
    if (m_renew_thread.joinable())
    {
        m_renew_thread.join();
    }
}

void KucoinTokenManager::renew_token()
{
    std::lock_guard guard(mutex_);
    // Logger::logger.log_special(LogLocation,false,"Renewing token from: %s",m_api_url.c_str());
    // std::cout << "Renewing token from: " << m_api_url << std::endl;
    CURL *curl = curl_easy_init();
    std::string readBuffer;

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, m_api_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 0L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        // std::cout << "CURL result code: " << res << std::endl;
        // std::cout << "CURL response: " << readBuffer << std::endl;

        if (res == CURLE_OK)
        {
            try
            {
                auto j = json::parse(readBuffer);
                if (j["code"] == "200000")
                {
                    std::lock_guard<std::mutex> lock(m_mutex);
                    m_token = j["data"]["token"];
                    m_endpoint = j["data"]["instanceServers"][0]["endpoint"];
                    // std::cout << "Token renewed: " << m_token << std::endl;
                    // std::cout << "Endpoint: " << m_endpoint << std::endl;
                }
                else
                {
                    Logger::logger.log_special(LogLocation, true, "Kucoin API error: %s", j.dump().c_str());
                    // std::cerr << "Kucoin API error: " << j.dump() << std::endl;
                }
            }
            catch (const std::exception &e)
            {
                Logger::logger.log_special(LogLocation, true, "JSON parse error: %s", e.what());
                // std::cerr << "JSON parse error: " << e.what() << std::endl;
            }
        }
        else
        {
            Logger::logger.log_special(LogLocation, true, "Failed to fetch token from Kucoin. CURL error code: %d", (int)res);
            // std::cerr << "Failed to fetch token from Kucoin. CURL error code: " << res << std::endl;
        }
    }
    else
    {
        Logger::logger.log_special(LogLocation, true, "Failed to initialize CURL.");
        // std::cerr << "Failed to initialize CURL." << std::endl;
    }
}