drop database Pokemon;
create database Pokemon;
use Pokemon;

create table City (
    name char(20),
    description char(30),
    primary key(name)
);

create table Trainer (
    id int auto_increment,
    name char(10),
    hometown char(20),
    primary key(id),
    foreign key(hometown) references City(name)
);

create table Gym (
    leader_id int,
    city char(20),
    foreign key(leader_id) references Trainer(id),
    foreign key(city) references City(name)
);

create table Pokemon (
    id int,
    name char(20),
    type char(15),
    primary key(id)
);

create table Evolution (
    before_id int,
    after_id int,
    foreign key(before_id) references Pokemon(id),
    foreign key(after_id) references Pokemon(id)
);

create table CatchedPokemon (
    id int auto_increment,
    owner_id int,
    pid int,
    level int,
    nickname char(30),
    primary key(id),
    foreign key(owner_id) references Trainer(id),
    foreign key(pid) references Pokemon(id)
);