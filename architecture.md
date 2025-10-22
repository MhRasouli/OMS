graph TD
    %% Define Styles for different component types
    classDef apiStyle fill:#D6EAF8,stroke:#3498DB,stroke-width:2px
    classDef serviceStyle fill:#D1F2EB,stroke:#1ABC9C,stroke-width:2px
    classDef execStyle fill:#E8DAEF,stroke:#8E44AD,stroke-width:2px
    classDef dataStyle fill:#FCF3CF,stroke:#F1C40F,stroke-width:2px
    classDef messageStyle fill:#FADBD8,stroke:#E74C3C,stroke-width:2px
    classDef externalStyle fill:#FDEDEC,stroke:#C0392B,stroke-width:2px
    classDef securityStyle fill:#E5E7E9,stroke:#839192,stroke-width:2px
    classDef infraStyle fill:#D5DBDB,stroke:#5D6D7E,stroke-width:2px

    subgraph ClientLayer["Client Layer"]
        WebApp["fa:fa-desktop Web Application (React/Vue)"]
        MobileApp["fa:fa-mobile-alt Mobile App (iOS/Android)"]
    end

    subgraph InfrastructureGateway["Infrastructure & Gateway Layer"]
        LB["fa:fa-server Load Balancer"]
        WAF["fa:fa-shield-alt Web Application Firewall"]
        API_GW["fa:fa-network-wired API Gateway"]
        AuthSvc["fa:fa-key Auth Service (OAuth2/JWT)"]

        LB --> WAF --> API_GW
        API_GW -- "Authenticates Request via" --> AuthSvc
    end

    subgraph OmsCore["Core Trading System (Microservices)"]
        TRP["fa:fa-cogs Trade Request Processor (Orchestrator)"]
        RiskSvc["fa:fa-exclamation-triangle Risk Management Service"]
        AnalyticsSvc["fa:fa-chart-bar Analytics & Reporting Service"]

        subgraph ExecutionEngine["Core Execution Engine"]
            direction LR
            MT["fa:fa-bolt Market Trader"]
            PF["fa:fa-rss Price Feeder"]
            MWS["fa:fa-wallet Market Wallet Supervisor"]
        end
    end

    subgraph DataMessagingLayer["Data, Messaging & Ops Layer"]
        MQ["fa:fa-exchange-alt Message Queue / Event Bus (Kafka/RabbitMQ)"]
        Cache["fa:fa-bolt In-Memory Cache (Redis)"]
        DB["fa:fa-database Primary DB (PostgreSQL)"]
        TSDB["fa:fa-chart-line Time-Series DB (TimescaleDB)"]
        DWH["fa:fa-warehouse Data Warehouse (BigQuery)"]
        Logs["fa:fa-file-alt Log Aggregator (ELK Stack)"]
        ConfigSvc["fa:fa-lock Config & Secrets Mgmt (Vault)"]
        Monitor["fa:fa-heartbeat Monitoring & Alerting (Prometheus)"]
    end

    subgraph ExchangeConnectors["Exchange Connectors (Abstraction Layer)"]
        direction LR
        BinanceConn["Binance Connector"]
        NobitexConn["Nobitex Connector"]
        KucoinConn["Kucoin Connector"]
    end

    subgraph ExternalAPIs["fa:fa-globe External Exchange APIs"]
        direction LR
        BinanceAPI["Binance API"]
        NobitexAPI["Nobitex API"]
        KucoinAPI["Kucoin API"]
    end

    %% --- FLOWS ---
    %% Client to Core System
    ClientLayer -- "HTTPS / WebSocket" --> LB
    API_GW -- "REST/gRPC API Calls" --> TRP

    %% Trade Orchestration Flow
    TRP -- "1. Validate Request" --> Cache & DB
    TRP -- "2. Pre-Trade Risk Check (gRPC)" --> RiskSvc
    RiskSvc -- "Checks Limits/Exposure" --> DB
    TRP -- "3. Publishes 'TRADE_REQUESTED' Event" -.-> MQ

    %% Asynchronous Execution Flow
    MQ -- "'TRADE_REQUESTED' Event" -.-> MT
    MT -- "Fetches API Keys" --> ConfigSvc
    MT -- "Executes Trade via" --> ExchangeConnectors
    MT -- "Writes Execution Record" --> DB
    MT -- "Publishes 'TRADE_EXECUTED' Event" -.-> MQ

    %% Market Data Flow
    PF -- "Subscribes to Market Data via" --> ExchangeConnectors
    ExchangeConnectors -- "WebSocket Market Data Stream" ==> PF
    PF -- "Publishes Price Ticks" -.-> MQ
    MQ -- "Price Ticks" -.-> TSDB & AnalyticsSvc

    %% Wallet Management Flow
    MWS -- "Periodically Queries Balances via" --> ExchangeConnectors
    MWS -- "Updates Wallet States" --> DB
    MQ -- "'TRADE_EXECUTED' Event" -.-> MWS & AnalyticsSvc & Logs

    %% Connector to External APIs
    ExchangeConnectors -- "REST API / WebSocket" --> ExternalAPIs

    %% Cross-Cutting Concerns (Monitoring, Logging)
    OmsCore -- "Pushes Metrics" -.-> Monitor
    DataMessagingLayer -- "Pushes Metrics" -.-> Monitor
    ExchangeConnectors -- "Pushes Metrics" -.-> Monitor
    TRP & RiskSvc & MT & PF & MWS -- "Writes Structured Logs" -.-> Logs
    AnalyticsSvc -- "Reads from" --> DWH
    DB & TSDB -- "ETL Process" --> DWH


    %% Apply Styles to Nodes
    class WebApp,MobileApp apiStyle;
    class LB,WAF,API_GW,AuthSvc infraStyle;
    class TRP,RiskSvc,AnalyticsSvc serviceStyle;
    class MT,PF,MWS execStyle;
    class DB,TSDB,DWH,Cache,Logs,ConfigSvc,Monitor dataStyle;
    class MQ messageStyle;
    class BinanceConn,NobitexConn,KucoinConn externalStyle;
    class BinanceAPI,NobitexAPI,KucoinAPI securityStyle;
