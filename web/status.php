<?php

$json = file_get_contents('pool.json');

$jsonIterator = json_decode($json, TRUE);
$updatetime = null;
$plvl = null;
$ptmp = null;
$atmp = null;
$arhu = null;
$rssi = null;
$pump = null;

foreach ($jsonIterator as $key => $val) {
	if ($key == "feeds") {
		foreach ($val as $key2 => $val2) {
			if ($val2["name"] == "poolLevel") {
				$plvl = $val2["last_value"];
			}
			if ($val2["name"] == "poolTemp") {
				$updatetime = $val2["last_value_at"];
				$ptmp = $val2["last_value"];
			}
			if ($val2["name"] == "airTemp") {
				$atmp = $val2["last_value"];
			}
			if ($val2["name"] == "airRH") {
				$arhu = $val2["last_value"];
			}
			if ($val2["name"] == "poolRSSI") {
				$rssi = $val2["last_value"];
			}
			if ($val2["name"] == "poolPump") {
				$pump = $val2["last_value"];
			}
		}
	}
}

$pf = $ptmp * 9/5 + 32;
$af = $atmp * 9/5 + 32;
?>

<html>
<body>
<table>
<tr><th>Air</th></tr>
<tr><td>temp:</td><td><?php echo number_format($af, 2, '.', ','); ?> *F</td></tr>
<tr><td>humidity:</td><td><?php echo number_format($arhu, 2, '.', ','); ?>%</td></tr>
<tr><th>Pool</th></tr>
<tr><td>temp:</td><td><?php echo number_format($pf, 2, '.', ','); ?> *F</td></tr>
<tr><td>level:</td><td><?php echo $plvl; ?></td></tr>
</table>



<?php

date_default_timezone_set('UTC');

$datetime1 = new DateTime($updatetime);
$datetime2 = new DateTime(date("c"));
$interval = $datetime1->diff($datetime2);

echo "<p>";
echo $interval->format("%H hours %I mins %S seconds old");
echo "</p>";
?>
</body>
</html>
