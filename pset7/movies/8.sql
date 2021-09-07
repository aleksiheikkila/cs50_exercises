--  list the names of all people who starred in Toy Story
select p.name
from people p
join stars s on s.person_id = p.id
join movies m on m.id = s.movie_id
where m.title = 'Toy Story'
;
