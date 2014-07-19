drop table m_sts_chronology;
create table m_sts_chronology
(
dataset              varchar(10) not null,
shuttle_mode         varchar(80),
shuttle_mode_number  number,
timestamp            DATE      not null,
metdd                number,
methh                number(2),
metmm                number(2),
metss                number(2),
angle_to_zenith      number(3),
power_step           number(3),
srdl_status          number(4),
hrdl_status          number(4),
daq_status           number(4)
);

commit;

