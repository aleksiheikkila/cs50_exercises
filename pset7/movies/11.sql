-- list the titles of the five highest rated movies (in order) that Chadwick Boseman starred in, starting with the highest rated.
select m.title
from movies m
join stars s on s.movie_id = m.id
join people p on p.id = s.person_id
join ratings r on r.movie_id = m.id
where p.name = 'Chadwick Boseman'
order by r.rating desc
limit 5
;