DROP INDEX `ALR__A_PUID` ON ALR;
CREATE INDEX `ALR__A_PUID` ON ALR (A_PUID(190));

DROP INDEX `PACKS__UID` ON PACKS;                                                
CREATE INDEX `PACKS__UID` ON PACKS (UID(190));

DROP TABLE `VER`;

CREATE TABLE `SCHEMA_CHANGES`(
   `ID` int NOT NULL AUTO_INCREMENT,
   `VERSION_NUMBER` int NOT NULL,
   `SCRIPT_NAME` varchar(255) NOT NULL,
   `TIMESTAMP_UTC_APPLIED` varchar(19) NOT NULL,
   PRIMARY KEY (ID)
);

INSERT INTO `SCHEMA_CHANGES` (`VERSION_NUMBER`, `SCRIPT_NAME`, `TIMESTAMP_UTC_APPLIED`)
VALUES ('1', 'updatealerts1.sql', UTC_TIMESTAMP());
