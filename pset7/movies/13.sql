-- list the names of all people who starred in a movie in which Kevin Bacon also starred
select p.name
from people p
join stars s on s.person_id = p.id
where s.movie_id in (select distinct m.id
                    from movies m
                    join stars s on s.movie_id = m.id
                    join people p on p.id = s.person_id
                    where p.name = 'Kevin Bacon'
                    and p.birth = 1958)
and p.name <> 'Kevin Bacon'
;

