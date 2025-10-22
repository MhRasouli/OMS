#pragma once

#include "stdafx.h"
#include "AThread.h"
#include "FeedCheckerThread.h"
#include "Logger.h"
#include "Configuration.h"
#include "Coin.h"
#include "GlobalVariables.h"
#include "PriceFeeder.h"
#include <cpr/cpr.h>
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#include <jsoncpp/json/writer.h>
#include <jsoncpp/json/value.h>

class NobitexFeeder : public PriceFeeder
{
public:
    ~NobitexFeeder();
    NobitexFeeder(Coin *Coin_ptr);
    Coin *coin_ptr;
    void StopGetData() override;

private:
    cpr::Session session;
    FeedCheckerThread fedchkThread;
    unsigned ThreadFunction();
    bool dataIsNew;
    void Loop_delay();
};