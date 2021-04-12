-- This is a Timedb table
DROP TABLE IF EXISTS observation CASCADE;
CREATE TABLE observation (
	"time" TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"timestamp" BIGINT,
	"ep" VARCHAR(64) NOT NULL,
	"object" INT NOT NULL,
	"instance" INT,
	"resource" INT DEFAULT 0,
	"value" TEXT
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
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"endpoint" VARCHAR(64) UNIQUE NOT NULL,
	"description" TEXT,
	"online" BOOLEAN NOT NULL
);

ALTER TABLE devices
ADD CONSTRAINT devices_uq
UNIQUE (endpoint);

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

select age(time, TO_TIMESTAMP(timestamp)),
       age(now(), time) from observation order by time desc;


INSERT INTO devices(endpoint, online) 
VALUES('test', false) 
ON CONFLICT (endpoint) 
DO UPDATE SET online = EXCLUDED.online;

DROP VIEW IF EXISTS temperature CASCADE;
CREATE VIEW temperature AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), ep, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3303
order by time desc;

DROP VIEW IF EXISTS sensors CASCADE;
CREATE VIEW sensors AS 
SELECT time,
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
	ep,
	CASE object
		WHEN 3300 THEN 'gas'
		WHEN 3303 THEN 'temperature'
		WHEN 3304 THEN 'humidity'
		WHEN 3325 THEN 'concentration'
		WHEN 3335 THEN 'colour'
		WHEN 10314 THEN 'particulate'
		ELSE 'unknown'
	END sensor,
	CAST(value AS decimal(38,2)),
	CASE object
		WHEN 3300 THEN 'ppm'
		WHEN 3303 THEN '°C'
		WHEN 3304 THEN '%RH'
		WHEN 3325 THEN 'ppm'
		WHEN 3335 THEN 'lux'
		WHEN 10314 THEN 'µg/m3'
		ELSE 'unknown'
	END unit
	FROM observation 
order by time desc;

DROP VIEW IF EXISTS thingy91ejka CASCADE;
CREATE VIEW thingy91ejka AS 
SELECT time,
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
	ep, 	CASE object
		WHEN 3300 THEN 'gas'
		WHEN 3303 THEN 'temperature'
		WHEN 3304 THEN 'humidity'
		WHEN 3325 THEN 'concentration'
		WHEN 3335 THEN 'colour'
		WHEN 10314 THEN 'particulate'
		ELSE 'unknown'
	END sensor,
	CAST(value AS decimal(38,2)),
	CASE object
		WHEN 3300 THEN 'ppm'
		WHEN 3303 THEN '°C'
		WHEN 3304 THEN '%RH'
		WHEN 3325 THEN 'ppm'
		WHEN 3335 THEN 'lux'
		WHEN 10314 THEN 'µg/m3'
		ELSE 'unknown'
	END unit
	FROM observation
WHERE ep = 'Thingy:91-EJKA'
order by time desc;

UPDATE temperature SET 
time = DATE_TRUNC('second', time),
age = DATE_TRUNC('second', age);


DROP VIEW IF EXISTS devicesview CASCADE;
CREATE VIEW devicesview AS 
SELECT time,
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
	endpoint,
	description,
	CASE online
		WHEN FALSE THEN 'offline'
		WHEN TRUE THEN 'online'
	END status
FROM devices
order by time desc;