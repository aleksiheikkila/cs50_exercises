-- list all movies released in 2010 and their ratings, in descending order by rating.
-- For movies with the same rating, order them alphabetically by title

select m.title, r.rating
from movies m
join ratings r
    -- keeps only those that are in the rating tbl
    on m.id = r.movie_id
where year = 2010
and r.rating is not null
order by r.rating desc, m.title
;
