-- This is a Timedb table
DROP TABLE IF EXISTS observation CASCADE;
CREATE TABLE observation (
	"time" TIMESTAMP WITH TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"timestamp" BIGINT,
	"endpoint" VARCHAR(64) NOT NULL,
	"object" INT NOT NULL,
	"instance" INT,
	"resource" INT DEFAULT 0,
	"value" TEXT
);

DROP TABLE IF EXISTS log CASCADE;
CREATE TABLE log (
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"endpoint" VARCHAR(64) NOT NULL,
	"ip" VARCHAR(20),
	"regid" VARCHAR(10),
	"message" VARCHAR(12),
	"error" TEXT
);

SELECT create_hypertable('observation', 'time');
SELECT create_hypertable('log', 'time');

DROP TABLE IF EXISTS devices CASCADE;
CREATE TABLE devices (
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"endpoint" VARCHAR(64) UNIQUE NOT NULL,
	"description" TEXT,
	"status" INT NOT NULL
);

ALTER TABLE devices
ADD CONSTRAINT devices_uq
UNIQUE (endpoint);

DROP TABLE IF EXISTS resources CASCADE;
CREATE TABLE resources (
	"id" SERIAL PRIMARY KEY,
	"time" TIMESTAMP WITHOUT TIME ZONE DEFAULT CURRENT_TIMESTAMP NOT NULL,
	"endpoint" VARCHAR(64) NOT NULL,
	"object" INT NOT NULL,
	"instance" INT DEFAULT 0,
	"resource" INT DEFAULT 0,
	"unit" VARCHAR(8)
);

select age(time, TO_TIMESTAMP(timestamp)),
       age(now(), time) from observation ORDER BY time desc;


INSERT INTO devices(endpoint, status, description) 
VALUES('gas-test', 2, 'Ozone sensor test') 
ON CONFLICT (endpoint) 
DO UPDATE SET 
status = EXCLUDED.status,
description = EXCLUDED.description;


UPDATE devices
SET description = 'Java Test Client'
WHERE endpoint = 'Thingy:91-Test';

DROP VIEW IF EXISTS gas_VOC CASCADE;
CREATE VIEW gas_VOC AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), endpoint, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3300 AND instance = 0
ORDER BY time desc;

DROP VIEW IF EXISTS gas CASCADE;
CREATE VIEW gas AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), endpoint, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3300 AND instance = 1
ORDER BY time desc;

DROP VIEW IF EXISTS temperature CASCADE;
CREATE VIEW temperature AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), endpoint, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3303
ORDER BY time desc;

DROP VIEW IF EXISTS humidity CASCADE;
CREATE VIEW humidity AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), endpoint, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3304
ORDER BY time desc;

DROP VIEW IF EXISTS concentration CASCADE;
CREATE VIEW concentration AS 
SELECT time, age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)), endpoint, CAST(value AS decimal(38,2)) FROM observation 
WHERE object = 3325
ORDER BY time desc;




DROP VIEW IF EXISTS particulate CASCADE;
CREATE VIEW particulate AS 
SELECT time,
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
	CASE instance
		WHEN 2 THEN endpoint || ' PM2.5'
		WHEN 3 THEN endpoint || ' PM10'
		WHEN 4 THEN endpoint || ' Typical PM'
	END ep_size,
	CAST(value AS decimal(38,2)),
	CASE object WHEN 3300 THEN 'ug/m3' END unit
FROM observation
WHERE object = 3300 AND instance > 1
ORDER BY time desc;

DROP VIEW IF EXISTS sensors CASCADE;
CREATE VIEW sensors AS 
SELECT time,
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
	endpoint,
	CASE object
		WHEN 3300 THEN
			CASE instance
				WHEN 0 THEN 'gas VOC'
				WHEN 1 THEN 'gas'
				WHEN 2 THEN 'PM2.5 particulate'
				WHEN 3 THEN 'PM10 particulate'
				WHEN 4 THEN 'Typical particulate'
			END
		WHEN 3303 THEN 'temperature'
		WHEN 3304 THEN 'humidity'
		WHEN 3325 THEN 'concentration'
		WHEN 3335 THEN 'colour'
		WHEN 10314 THEN 'particulate'
		ELSE 'unknown'
	END sensor,
	CAST(value AS decimal(38,2)),
	CASE object
		WHEN 3300 THEN 'ppb'
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
	endpoint,
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
		WHEN 3300 THEN 'ppb'
		WHEN 3303 THEN '°C'
		WHEN 3304 THEN '%RH'
		WHEN 3325 THEN 'ppm'
		WHEN 3335 THEN 'lux'
		WHEN 10314 THEN 'µg/m3'
		ELSE 'unknown'
	END unit
	FROM observation
WHERE endpoint = 'Thingy:91-EJKA'
order by time desc;

UPDATE temperature SET 
time = DATE_TRUNC('second', time),
age = DATE_TRUNC('second', age);


DROP VIEW IF EXISTS devicesview CASCADE;
CREATE VIEW devicesview AS 
SELECT 
	time,
<<<<<<< HEAD
	age(DATE_TRUNC('second', now() - INTERVAL '1 hour'), DATE_TRUNC('second', time)) ,
=======
	age(DATE_TRUNC('second', now()), DATE_TRUNC('second', time)),
>>>>>>> 9556cf42d1f6f0fdd14da0595623b43f1bc25a38
	endpoint,
	description,
	status
FROM devices
order by time desc;

SELECT age FROM devicesview ;
SELECT MAX(age) - INTERVAL '1 hour' from devicesview;