-- Keep a log of any SQL queries you execute as you solve the mystery.

-- Foreword: these queries could be tied together more comprehensively and in a nicer way.
-- But it works ok as is, so didn't start to post-produce it :)


-- check DB tables with .tables
--airports                  crime_scene_reports       people
--atm_transactions          flights                   phone_calls
--bank_accounts             interviews
--courthouse_security_logs  passengers

-- explore crime scene report
select * from crime_scene_reports limit 10;

-- Check the date of interest
select * from crime_scene_reports
where year = 2020
and month = 7
and day = 28
;
-- >
--  Theft of the CS50 duck took place at 10:15am at the Chamberlin Street courthouse.
-- Interviews were conducted today with three witnesses who were present at the time —
-- each of their interview transcripts mentions the courthouse.

-- let's check interviews
-- explore
select * from interviews limit 3;
-- schema: id | name | year | month | day | transcript


select * from interviews
where year = 2020
and month = 7
and day = 28
;

-- These are the interesting ones:

-- 161 | Ruth | 2020 | 7 | 28 |
-- Sometime within ten minutes of the theft, I saw the thief get into a car in the courthouse parking lot and drive away.
-- If you have security footage from the courthouse parking lot, you might want to look for cars that left the parking lot in that time frame.

--> 1. check courthouse_security_logs between 10:15 and 10:25 am, for cars leaving the place

select * from courthouse_security_logs
where year = 2020
and month = 7
and day = 28
and hour = 10
and activity = 'exit'
and minute >= 15 and minute <= 25;

--> got these license plates
--id | year | month | day | hour | minute | activity | license_plate
--260 | 2020 | 7 | 28 | 10 | 16 | exit | 5P2BI95
--261 | 2020 | 7 | 28 | 10 | 18 | exit | 94KL13X
--262 | 2020 | 7 | 28 | 10 | 18 | exit | 6P58WS2
--263 | 2020 | 7 | 28 | 10 | 19 | exit | 4328GD8
--264 | 2020 | 7 | 28 | 10 | 20 | exit | G412CB7
--265 | 2020 | 7 | 28 | 10 | 21 | exit | L93JTIZ
--266 | 2020 | 7 | 28 | 10 | 23 | exit | 322W7JE
--267 | 2020 | 7 | 28 | 10 | 23 | exit | 0NTHK55



-- 162 | Eugene | 2020 | 7 | 28 |
-- I don't know the thief's name, but it was someone I recognized. Earlier this morning, before I arrived at the courthouse,
-- I was walking by the ATM on Fifer Street and saw the thief there withdrawing some money.

--> 2. check atm records
select *
from atm_transactions
where year = 2020
and month = 7
and day = 28
and transaction_type = 'withdraw'
and atm_location = 'Fifer Street';

-->
--id  | account_number | year | month | day | atm_location | transaction_type | amount
--246 | 28500762 | 2020 | 7 | 28 | Fifer Street | withdraw | 48
--264 | 28296815 | 2020 | 7 | 28 | Fifer Street | withdraw | 20
--266 | 76054385 | 2020 | 7 | 28 | Fifer Street | withdraw | 60
--267 | 49610011 | 2020 | 7 | 28 | Fifer Street | withdraw | 50
--269 | 16153065 | 2020 | 7 | 28 | Fifer Street | withdraw | 80
--288 | 25506511 | 2020 | 7 | 28 | Fifer Street | withdraw | 20
--313 | 81061156 | 2020 | 7 | 28 | Fifer Street | withdraw | 30
--336 | 26013199 | 2020 | 7 | 28 | Fifer Street | withdraw | 35



-- join Bank account info and person info, filter using the above licence plate nbrs
select atm.*, acc.person_id, acc.creation_year, p.name, p.phone_number, p.passport_number, p.license_plate
from atm_transactions atm
left join bank_accounts acc using (account_number)
left join people p on p.id = acc.person_id
where year = 2020
and month = 7
and day = 28
and transaction_type = 'withdraw'
and atm_location = 'Fifer Street'
and p.license_plate in (
    select license_plate from courthouse_security_logs
    where year = 2020
    and month = 7
    and day = 28
    and hour = 10
    and activity = 'exit'
    and minute >= 15 and minute <= 25);

-->
246 | 28500762 | 2020 | 7 | 28 | Fifer Street | withdraw | 48 | 467400 | 2014 | Danielle | (389) 555-5198 | 8496433585 | 4328GD8
267 | 49610011 | 2020 | 7 | 28 | Fifer Street | withdraw | 50 | 686048 | 2010 | Ernest | (367) 555-5533 | 5773159633 | 94KL13X
288 | 25506511 | 2020 | 7 | 28 | Fifer Street | withdraw | 20 | 396669 | 2014 | Elizabeth | (829) 555-5269 | 7049073643 | L93JTIZ
336 | 26013199 | 2020 | 7 | 28 | Fifer Street | withdraw | 35 | 514354 | 2012 | Russell | (770) 555-1861 | 3592750733 | 322W7JE


-- 163 | Raymond | 2020 | 7 | 28 |
-- As the thief was leaving the courthouse, they called someone who talked to them for less than a minute.
-- In the call, I heard the thief say that they were planning to take the earliest flight out of Fiftyville tomorrow.
-- The thief then asked the person on the other end of the phone to purchase the flight ticket.

--> 3. check phone_calls
select * from phone_calls
where year = 2020
and month = 7
and day = 28
and duration < 60;

-- add the caller numbers to the above query to narrow it further down
select atm.*, acc.person_id, acc.creation_year, p.name, p.phone_number, p.passport_number, p.license_plate
from atm_transactions atm
left join bank_accounts acc using (account_number)
left join people p on p.id = acc.person_id
where year = 2020
and month = 7
and day = 28
and transaction_type = 'withdraw'
and atm_location = 'Fifer Street'
and p.license_plate in (
    select license_plate from courthouse_security_logs
    where year = 2020
    and month = 7
    and day = 28
    and hour = 10
    and activity = 'exit'
    and minute >= 15 and minute <= 25)
and p.phone_number in (
    select caller from phone_calls
    where year = 2020
    and month = 7
    and day = 28
    and duration < 60)
;

-->
267 | 49610011 | 2020 | 7 | 28 | Fifer Street | withdraw | 50 | 686048 | 2010 | Ernest | (367) 555-5533 | 5773159633 | 94KL13X
336 | 26013199 | 2020 | 7 | 28 | Fifer Street | withdraw | 35 | 514354 | 2012 | Russell | (770) 555-1861 | 3592750733 | 322W7JE

--> 4. check what is the earliest flight the next morning
select * from airports where full_name like '%iftyville%';
--> id = 8

select f.*,
from_a.abbreviation as orig_abbr, from_a.full_name as orig_name,
to_a.abbreviation as dest_abbr, to_a.full_name as dest_name
from flights f
left join airports from_a on from_a.id = f.origin_airport_id
left join airports to_a on to_a.id = f.destination_airport_id
where year = 2020
and month = 7
and day = 29
and origin_airport_id = 8
order by hour asc, minute asc;

-- Earliest flight is to London, LHR | Heathrow Airport. Flight id 36
36 | 8 | 4 | 2020 | 7 | 29 | 8 | 20 | CSF | Fiftyville Regional Airport | LHR | Heathrow Airport


--> 5. check passengers in that flight

select * from passengers
where flight_id = 36
and passport_number in (5773159633, 3592750733);

--> 36 | 5773159633 | 4A
-- Thus, it seems likely that Ernest is the thief, and he headed to London.

-- WHo is the accomplice?


-- check phone call with the right caller
select * from phone_calls
where year = 2020
and month = 7
and day = 28
and duration < 60
and caller = '(367) 555-5533';

--> receiver (375) 555-8161

select name from people where phone_number = '(375) 555-8161';

--> Accomplice is Berthold!