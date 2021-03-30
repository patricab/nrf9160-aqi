-- This is a Timedb table
DROP TABLE IF EXISTS observation CASCADE;
CREATE TABLE observation (
	"time" TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"timestamp" BIGINT,
	"ep" VARCHAR(64) NOT NULL,
	"object" INT NOT NULL,
	"instance" INT,
	"resource" INT DEFAULT 0,
	"value" DOUBLE PRECISION
);

DROP TABLE IF EXISTS log CASCADE;
CREATE TABLE log (
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"ep" VARCHAR(64) NOT NULL,
	"ip" VARCHAR(20),
	"regid" VARCHAR(10),
	"error" TEXT
);

SELECT create_hypertable('observation', 'time');
SELECT create_hypertable('log', 'time');

DROP TABLE IF EXISTS devices CASCADE;
CREATE TABLE devices (
	"id" SERIAL PRIMARY KEY,
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"ep" VARCHAR(64) NOT NULL,
	"description" TEXT
);

DROP TABLE IF EXISTS resources CASCADE;
CREATE TABLE resources (
	"id" SERIAL PRIMARY KEY,
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"ep" VARCHAR(64) NOT NULL,
	"object" INT NOT NULL,
	"instance" INT DEFAULT 0,
	"resource" INT DEFAULT 0,
	"unit" VARCHAR(8)
);
