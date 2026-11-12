
import type { Alert, Road, RouteResult, Weather } from '../types'

// Read the backend URL from frontend/.env.
// Remove trailing slashes to prevent duplicate slashes in API URLs.
const API_BASE_URL = (
  import.meta.env.VITE_API_BASE_URL ?? ''
).replace(/\/+$/, '')

export interface RouteRequest {
  startId: string
  destinationId: string
  startCoordinates?: [number, number]
  destinationCoordinates?: [number, number]
}

async function request<T>(
  path: string,
  options?: RequestInit,
): Promise<T> {
  const headers = new Headers(options?.headers)

  // Send JSON Content-Type only when a request body exists.
  // This avoids unnecessary CORS preflight requests for GET calls.
  if (options?.body != null && !headers.has('Content-Type')) {
    headers.set('Content-Type', 'application/json')
  }

  if (!headers.has('Accept')) {
    headers.set('Accept', 'application/json')
  }

  const url = `${API_BASE_URL}${path}`

  let response: Response

  try {
    response = await fetch(url, {
      ...options,
      headers,
    })
  } catch (error) {
    console.error(`Network/CORS error for ${path}:`, error)

    throw new Error(
      `Failed to connect to the backend for ${path}. Check the backend server, Codespaces port forwarding and CORS configuration.`
    )
  }

  // Detect GitHub Codespaces authentication redirects.
  if (response.redirected) {
    throw new Error(
      `Backend request was redirected to ${response.url}. Check Codespaces port visibility and authentication.`
    )
  }

  const responseText = await response.text()

  if (!response.ok) {
    let message = responseText

    try {
      const parsed = JSON.parse(responseText)

      if (parsed.error) {
        message = parsed.error
      } else if (parsed.message) {
        message = parsed.message
      }
    } catch {
      // Keep the original response text if it is not JSON.
    }

    throw new Error(
      `HTTP ${response.status}: ${message || 'Request failed'}`
    )
  }

  if (!responseText.trim()) {
    throw new Error(`Empty response received from ${path}`)
  }

  try {
    return JSON.parse(responseText) as T
  } catch {
    throw new Error(
      `Invalid JSON response from ${path}. Check the backend API response.`
    )
  }
}

export const api = {
  getRoads: () =>
    request<Road[]>('/api/roads'),

  getWeather: () =>
    request<Weather>('/api/weather'),

  getAlerts: () =>
    request<Alert[]>('/api/alerts'),

  getRoute: (data: RouteRequest) =>
    request<RouteResult>('/api/route', {
      method: 'POST',
      body: JSON.stringify(data),
    }),
}
