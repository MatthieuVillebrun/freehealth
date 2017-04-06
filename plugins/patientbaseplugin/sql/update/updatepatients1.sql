ALTER TABLE `PATIENT_IDENTITY`
ADD COLUMN `MOBILE_PHONE` varchar(32),
ADD COLUMN `WORK_PHONE` varchar(32);

CREATE TABLE `SCHEMA_CHANGES`(
   `ID` int NOT NULL AUTO_INCREMENT,
   `VERSION_NUMBER` int NOT NULL,
   `SCRIPT_NAME` varchar(255) NOT NULL,
   `TIMESTAMP_UTC_APPLIED` varchar(19) NOT NULL,
   PRIMARY KEY (ID)
);

DROP TABLE `VERSION`;

INSERT INTO `SCHEMA_CHANGES` (`VERSION_NUMBER`, `SCRIPT_NAME`, `TIMESTAMP_UTC_APPLIED`)
VALUES ('1', 'patients1.sql', UTC_TIMESTAMP());
