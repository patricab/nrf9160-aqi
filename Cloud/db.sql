CREATE TABLE public.test (
    "time" timestamp without time zone DEFAULT CURRENT_TIMESTAMP NOT NULL,
    topic text NOT NULL,
    msg double precision
);