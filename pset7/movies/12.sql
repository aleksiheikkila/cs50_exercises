-- list the titles of all movies in which both Johnny Depp and Helena Bonham Carter starred.
select title from (
select m.title, count(*)
from movies m
join stars s on s.movie_id = m.id
join people p on p.id = s.person_id
where p.name in ('Johnny Depp', 'Helena Bonham Carter')
group by m.title
having count(*) = 2
)
;