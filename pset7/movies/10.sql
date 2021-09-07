-- list the names of all people who have directed a movie that received a rating of at least 9.0
select distinct name
from people p
join directors d on d.person_id = p.id
--join movies m on m.id = d.movie_id
join ratings r on r.movie_id = d.movie_id
where r.rating >= 9.0
;