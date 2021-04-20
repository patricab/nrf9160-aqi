package thingysim;

import java.math.BigDecimal;
import java.math.RoundingMode;
import java.util.Arrays;
import java.util.Date;
import java.util.List;
import java.util.Random;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import org.eclipse.leshan.client.resource.BaseInstanceEnabler;
import org.eclipse.leshan.client.servers.ServerIdentity;
import org.eclipse.leshan.core.Destroyable;
import org.eclipse.leshan.core.model.ObjectModel;
import org.eclipse.leshan.core.response.ExecuteResponse;
import org.eclipse.leshan.core.response.ReadResponse;
import org.eclipse.leshan.core.util.NamedThreadFactory;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class RandomPressureSensor extends BaseInstanceEnabler implements Destroyable {

	private static final Logger LOG = LoggerFactory.getLogger(RandomPressureSensor.class);

	private static final String UNIT_MICROGRAM_PER_CUBIC_METER = "hPa";
	private static final float MAX_RANGE_VALUE = 1100;
	private static final float MIN_RANGE_VALUE = 300;
	private static final int SENSOR_VALUE = 5700;
	private static final int UNITS = 5701;
	private static final int MAX_MEASURED_VALUE = 5602;
	private static final int MIN_MEASURED_VALUE = 5601;
	private static final int MAX_RANGE_VALUE_ID = 5604;
	private static final int MIN_RANGE_VALUE_ID = 5603;
	private static final int RESET_MIN_MAX_MEASURED_VALUES = 5605;
	private static final int TIMESTAMP = 5518;
	private static final List<Integer> supportedResources
	      = Arrays.asList(SENSOR_VALUE, UNITS, MAX_MEASURED_VALUE, MIN_MEASURED_VALUE,
	                      MAX_RANGE_VALUE_ID, MIN_RANGE_VALUE_ID,
	                      RESET_MIN_MAX_MEASURED_VALUES, TIMESTAMP);
	private final ScheduledExecutorService scheduler;
	private final Random rng = new Random();
	private Date timestamp;
	private float currentPressure = 1000f;
	private float minMeasuredValue = currentPressure;
	private float maxMeasuredValue = currentPressure;

	public RandomPressureSensor() {
		this.scheduler = Executors.newSingleThreadScheduledExecutor(new NamedThreadFactory("Pressure Sensor"));
		scheduler.scheduleAtFixedRate(new Runnable() {

			@Override
			public void run() {
				adjustPressure();
			}
		}, 2, 30, TimeUnit.SECONDS);
	}

	@Override
	public synchronized ReadResponse read(ServerIdentity identity, int resourceId) {
		LOG.info("Read on Pressure resource /{}/{}/{}", getModel().id, getId(), resourceId);
		switch (resourceId) {
		case MIN_MEASURED_VALUE:
			return ReadResponse.success(resourceId, getTwoDigitValue(minMeasuredValue));
		case MAX_MEASURED_VALUE:
			return ReadResponse.success(resourceId, getTwoDigitValue(maxMeasuredValue));
		case SENSOR_VALUE:
			return ReadResponse.success(resourceId, getTwoDigitValue(currentPressure));
		case UNITS:
			return ReadResponse.success(resourceId, UNIT_MICROGRAM_PER_CUBIC_METER);
		case TIMESTAMP:
			return ReadResponse.success(resourceId, getTimestamp());
		case MAX_RANGE_VALUE_ID:
			return ReadResponse.success(resourceId, MAX_RANGE_VALUE);
		case MIN_RANGE_VALUE_ID:
			return ReadResponse.success(resourceId, MIN_RANGE_VALUE);
		default:
			return super.read(identity, resourceId);
		}
	}

	@Override
	public synchronized ExecuteResponse execute(ServerIdentity identity, int resourceId, String params) {
		LOG.info("Execute on Pressure resource /{}/{}/{}", getModel().id, getId(), resourceId);
		switch (resourceId) {
		case RESET_MIN_MAX_MEASURED_VALUES:
			resetMinMaxMeasuredValues();
			return ExecuteResponse.success();
		default:
			return super.execute(identity, resourceId, params);
		}
	}

	private float getTwoDigitValue(float value) {
		BigDecimal toBeTruncated = BigDecimal.valueOf(value);
		return toBeTruncated.setScale(2, RoundingMode.HALF_UP).floatValue();
	}

	private void adjustPressure() {
		float delta = (rng.nextInt(50) - 25) / 10f;
		currentPressure += delta;
		if (currentPressure < 0.0f) {
			currentPressure -= delta;
		}
		Integer changedResource = adjustMinMaxMeasuredValue(currentPressure);
		timestamp = new Date();
		if (changedResource != null) {
			fireResourcesChange(SENSOR_VALUE, changedResource);
		} else {
			fireResourcesChange(SENSOR_VALUE);
		}
	}

	private synchronized Integer adjustMinMaxMeasuredValue(float newPressure) {
		if (newPressure > maxMeasuredValue) {
			maxMeasuredValue = newPressure;
			return MAX_MEASURED_VALUE;
		} else if (newPressure < minMeasuredValue) {
			minMeasuredValue = newPressure;
			return MIN_MEASURED_VALUE;
		} else {
			return null;
		}
	}

	private void resetMinMaxMeasuredValues() {
		minMeasuredValue = currentPressure;
		maxMeasuredValue = currentPressure;
	}

	private Date getTimestamp() {
		return timestamp;
	}

	@Override
	public List<Integer> getAvailableResourceIds(ObjectModel model) {
		return supportedResources;
	}

	@Override
	public void destroy() {
		scheduler.shutdown();
	}
}
