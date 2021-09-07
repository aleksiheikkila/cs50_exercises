-- determine the number of movies with an IMDb rating of 10.0
select count(*)
from ratings
where rating = 10.0
;