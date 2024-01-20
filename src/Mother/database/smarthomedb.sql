SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
START TRANSACTION;
SET time_zone = "+00:00";

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8mb4 */;


CREATE TABLE `devices` (
  `id` int(11) NOT NULL,
  `device_name` text DEFAULT NULL,
  `type` text DEFAULT NULL,
  `secret_key` varchar(32) DEFAULT NULL,
  `user_id` int(11) DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO `devices` (`id`, `device_name`, `type`, `secret_key`, `user_id`) VALUES
(5, 'xD', 'RGBController', 'zSWGGF3O0780fYU2aZH9lJPkOl68VP8m', 3),
(6, 'xD', 'RGBController', 'ZnRrpfwvIW1ZloqJRMGmbgI1yXgmuiK2', 3);

CREATE TABLE `users` (
  `id` int(11) NOT NULL,
  `login` text DEFAULT NULL,
  `password` text DEFAULT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_general_ci;

INSERT INTO `users` (`id`, `login`, `password`) VALUES
(2, 'adamoss', 'adamoss'),
(3, 'adamoss2', 'adamoss'),
(4, 'adamoss4', 'adamoss');


ALTER TABLE `devices`
  ADD PRIMARY KEY (`id`),
  ADD KEY `fk_user_device` (`user_id`);

ALTER TABLE `users`
  ADD PRIMARY KEY (`id`);


ALTER TABLE `devices`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=7;

ALTER TABLE `users`
  MODIFY `id` int(11) NOT NULL AUTO_INCREMENT, AUTO_INCREMENT=6;


ALTER TABLE `devices`
  ADD CONSTRAINT `fk_user_device` FOREIGN KEY (`user_id`) REFERENCES `users` (`id`) ON DELETE CASCADE;
COMMIT;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
