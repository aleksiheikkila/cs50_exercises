-- list the names of all people who starred in a movie released in 2004, ordered by birth year
select name
from people p
join stars s on s.person_id = p.id
join movies m on m.id = s.movie_id
where m.year = 2004
order by p.birth
;