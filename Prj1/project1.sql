USE Pokemon;

# 1
SELECT name
FROM Trainer AS T
INNER JOIN (
    SELECT owner_id, COUNT(*) AS 'NUM'
    FROM CatchedPokemon 
    GROUP BY owner_id 
    HAVING COUNT(*) >= 3
) AS C
ON T.id = C.owner_id
ORDER BY NUM DESC;

# 2
SELECT P1.name
FROM Pokemon AS P1
INNER JOIN (
    SELECT type
    FROM Pokemon
    GROUP BY type
    ORDER BY COUNT(*) DESC
    LIMIT 2
) AS P2
ON P1.type = P2.type
ORDER BY P1.name ASC;

# 3
SELECT name
FROM Pokemon
WHERE name LIKE '_o%'
ORDER BY name ASC;

# 4
SELECT nickname
FROM CatchedPokemon
WHERE level >= 50
ORDER BY nickname ASC;

# 5
SELECT name
FROM Pokemon
WHERE LENGTH(name) = 6
ORDER BY name ASC;

# 6
SELECT name
FROM Trainer
WHERE hometown = 'Blue City'
ORDER BY name ASC;

# 7
SELECT DISTINCT hometown
FROM Trainer
ORDER BY hometown ASC;

# 8
SELECT AVG(level)
FROM CatchedPokemon
WHERE owner_id IN (
    SELECT id
    FROM Trainer
    WHERE name = 'Red'
);

# 9
SELECT nickname
FROM CatchedPokemon
WHERE nickname < 'T' OR nickname >= 'U'
ORDER BY nickname ASC;

# 10
SELECT nickname
FROM CatchedPokemon
WHERE level >= 50 AND owner_id >= 6
ORDER BY nickname ASC;

# 11
SELECT id, name
FROM Pokemon
ORDER BY id ASC;

# 12
SELECT nickname
FROM CatchedPokemon
WHERE level <= 50
ORDER BY level ASC;

# 13
SELECT name, id
FROM Pokemon
INNER JOIN (
    SELECT pid
    FROM CatchedPokemon
    INNER JOIN (
        SELECT id
        FROM Trainer
        WHERE hometown = 'Sangnok City'
    ) AS T
    ON T.id = owner_id
) AS C
ON id = C.pid
ORDER BY id ASC;

# 14
SELECT nickname
FROM CatchedPokemon
WHERE owner_id IN (
    SELECT T.id
    FROM Trainer AS T
    WHERE T.id IN (
        SELECT leader_id
        FROM Gym
    )
)
AND pid IN (
    SELECT P.id
    FROM Pokemon AS P
    WHERE type = 'Water'
)
ORDER BY nickname ASC;

# 15
SELECT COUNT(*)
FROM CatchedPokemon
WHERE pid IN (
    SELECT DISTINCT before_id
    FROM Evolution
);

# 16
SELECT COUNT(*)
FROM Pokemon
WHERE type IN (
    'Water', 'Electric', 'Psychic'
);

# 17
SELECT COUNT(DISTINCT pid)
FROM CatchedPokemon
WHERE owner_id IN (
    SELECT id
    FROM Trainer
    WHERE hometown = 'Sangnok City'
);

# 18
SELECT MAX(level)
FROM CatchedPokemon
WHERE owner_id IN (
    SELECT id
    FROM Trainer
    WHERE hometown = 'Sangnok City'
);

# 19
SELECT COUNT(DISTINCT type)
FROM Pokemon
WHERE id IN (
    SELECT pid
    FROM CatchedPokemon
    WHERE owner_id IN (
        SELECT leader_id
        FROM Gym
        WHERE city = 'Sangnok City'
    )
);

# 20
SELECT name, COUNT(*)
FROM Trainer AS T
INNER JOIN CatchedPokemon AS C
ON T.id = C.owner_id
WHERE hometown = 'Sangnok City'
GROUP BY T.id
ORDER BY COUNT(*) ASC;

# 21
SELECT name
FROM Pokemon
WHERE LEFT(LOWER(name), 1) IN (
    'a', 'e', 'i', 'o', 'u'
);

# 22
SELECT type, COUNT(*)
FROM Pokemon
GROUP BY type
ORDER BY COUNT(*), type ASC;

# 23
SELECT DISTINCT name
FROM Trainer
WHERE id IN (
    SELECT owner_id
    FROM CatchedPokemon
    WHERE level <= 10
)
ORDER BY name;

# 24
SELECT C.name, AVG(level)
FROM City AS C
INNER JOIN Trainer AS T
ON hometown = C.name
INNER JOIN CatchedPokemon
ON T.id = owner_id
GROUP BY C.name
ORDER BY AVG(level) ASC;

# 25
SELECT DISTINCT name
FROM Pokemon
WHERE id IN (
    SELECT pid
    FROM CatchedPokemon
    WHERE owner_id IN (
        SELECT id
        FROM Trainer
        WHERE hometown = 'Sangnok City'
    )
)
AND id IN (
    SELECT pid
    FROM CatchedPokemon
    WHERE owner_id IN (
        SELECT id
        FROM Trainer
        WHERE hometown = 'Brown City'
    )
)
ORDER BY name ASC;

# 26
SELECT name
FROM Pokemon
WHERE id IN (
    SELECT pid
    FROM CatchedPokemon
    WHERE nickname LIKE '% %'
)
ORDER BY name DESC;

# 27
SELECT name, MAX(level)
FROM Trainer AS T
INNER JOIN CatchedPokemon AS C
ON T.id = C.owner_id
GROUP BY T.id
HAVING COUNT(*) >= 4
ORDER BY name ASC;

# 28
SELECT name, AVG(level)
FROM Trainer AS T
INNER JOIN (
    SELECT owner_id, level
    FROM CatchedPokemon
    WHERE pid IN (
        SELECT id
        FROM Pokemon
        WHERE type IN (
            'Normal', 'Electric'
        )
    )
) AS C
ON T.id = owner_id
GROUP BY T.id
ORDER BY AVG(level) ASC;

# 29
SELECT P.name, T.name, description
FROM (
    SELECT id, name
    FROM Pokemon
    WHERE id = 152
) AS P
INNER JOIN CatchedPokemon
ON pid = P.id
INNER JOIN Trainer AS T
ON owner_id = T.id
INNER JOIN City AS C
ON hometown = C.name
ORDER BY level DESC;

# 30
SELECT E1.before_id AS 'id', 
    P1.name AS 'name 1', 
    P2.name AS 'name 2', 
    P3.name As 'name 3'
FROM Evolution AS E1
INNER JOIN Evolution AS E2
ON E1.after_id = E2.before_id
INNER JOIN Pokemon P1
ON P1.id = E1.before_id
INNER JOIN Pokemon P2
ON P2.id = E1.after_id
INNER JOIN Pokemon P3
ON P3.id = E2.after_id
ORDER BY E1.before_id ASC;

# 31
SELECT name
FROM Pokemon
WHERE id >= 10 AND id < 100
ORDER BY name;

# 32
SELECT name
FROM Pokemon
WHERE id NOT IN (
    SELECT pid
    FROM CatchedPokemon
)
ORDER BY name;

# 33
SELECT SUM(level)
FROM CatchedPokemon
WHERE owner_id IN (
    SELECT id
    FROM Trainer
    WHERE name = 'Matis'
);

# 34
SELECT name
FROM Trainer
WHERE id IN (
    SELECT leader_id
    FROM Gym
)
ORDER BY name;

# 35
SELECT P4.type, P4.cnt / P3.cnt * 100
FROM (
    SELECT COUNT(type) AS cnt
    FROM Pokemon
) AS P3, (
    SELECT type, cnt
    FROM (
        SELECT type, COUNT(type) AS cnt
        FROM Pokemon
        GROUP BY type
    ) AS P1
    WHERE P1.cnt IN (
        SELECT MAX(cnt)
        FROM (
            SELECT type, COUNT(type) AS cnt
            FROM Pokemon
            GROUP BY type
        ) AS P2
    )
) AS P4;

# 36
SELECT name
FROM Trainer
WHERE id IN (
    SELECT owner_id
    FROM CatchedPokemon
    WHERE nickname LIKE 'A%'
)
ORDER BY name;

# 37
SELECT name, I.sum
FROM Trainer AS T
INNER JOIN (
    SELECT owner_id, sum
    FROM (
        SELECT owner_id, SUM(level) AS sum
        FROM CatchedPokemon
        GROUP BY owner_id
    ) AS tmp2
    WHERE sum IN (
        SELECT MAX(sum)
        FROM (
            SELECT SUM(level) AS sum
            FROM CatchedPokemon
            GROUP BY owner_id
        ) AS tmp
    )
) AS I
ON T.id = I.owner_id;

# 38
SELECT name
FROM Pokemon
WHERE id IN (
    SELECT after_id
    FROM Evolution
    WHERE after_id NOT IN (
        SELECT after_id
        FROM Evolution
        WHERE before_id IN (
            SELECT after_id
            FROM Evolution
        )
    )
)
ORDER BY name;

# 39
SELECT name
FROM Trainer
WHERE id IN (
    SELECT owner_id
    FROM CatchedPokemon
    GROUP BY owner_id, pid
    HAVING COUNT(*) >= 2
)
ORDER BY name;

# 40
SELECT T1.hometown, T1.nickname
FROM (
    SELECT T.hometown, level, nickname
    FROM Trainer AS T
    INNER JOIN CatchedPokemon AS C
    ON T.id = C.owner_id
) AS T1
INNER JOIN (
    SELECT T.hometown, MAX(level) AS max_level
    FROM Trainer AS T
    INNER JOIN CatchedPokemon AS C
    ON T.id = C.owner_id
    GROUP BY T.hometown
) AS T2
ON T1.hometown = T2.hometown
AND T1.level = T2.max_level
ORDER BY T1.hometown;