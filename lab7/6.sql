-- lists the names of songs that are by Post Malone
SELECT name
FROM songs
WHERE artist_id = (SELECT id FROM artists where name = 'Post Malone' LIMIT 1)
;